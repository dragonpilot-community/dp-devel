#include "common/params.h"

#include <dirent.h>
#include <sys/file.h>

#include <algorithm>
#include <csignal>
#include <unordered_map>

#include "common/swaglog.h"
#include "common/util.h"
#include "system/hardware/hw.h"

namespace {

volatile sig_atomic_t params_do_exit = 0;
void params_sig_handler(int signal) {
  params_do_exit = 1;
}

int fsync_dir(const std::string &path) {
  int result = -1;
  int fd = HANDLE_EINTR(open(path.c_str(), O_RDONLY, 0755));
  if (fd >= 0) {
    result = fsync(fd);
    close(fd);
  }
  return result;
}

bool create_params_path(const std::string &param_path, const std::string &key_path) {
  // Make sure params path exists
  if (!util::file_exists(param_path) && !util::create_directories(param_path, 0775)) {
    return false;
  }

  // See if the symlink exists, otherwise create it
  if (!util::file_exists(key_path)) {
    // 1) Create temp folder
    // 2) Symlink it to temp link
    // 3) Move symlink to <params>/d

    std::string tmp_path = param_path + "/.tmp_XXXXXX";
    // this should be OK since mkdtemp just replaces characters in place
    char *tmp_dir = mkdtemp((char *)tmp_path.c_str());
    if (tmp_dir == NULL) {
      return false;
    }

    std::string link_path = std::string(tmp_dir) + ".link";
    if (symlink(tmp_dir, link_path.c_str()) != 0) {
      return false;
    }

    // don't return false if it has been created by other
    if (rename(link_path.c_str(), key_path.c_str()) != 0 && errno != EEXIST) {
      return false;
    }
  }

  return true;
}

std::string ensure_params_path(const std::string &prefix, const std::string &path = {}) {
  std::string params_path = path.empty() ? Path::params() : path;
  if (!create_params_path(params_path, params_path + prefix)) {
    throw std::runtime_error(util::string_format("Failed to ensure params path, errno=%d", errno));
  }
  return params_path;
}

class FileLock {
public:
  FileLock(const std::string &fn) {
    fd_ = HANDLE_EINTR(open(fn.c_str(), O_CREAT, 0775));
    if (fd_ < 0 || HANDLE_EINTR(flock(fd_, LOCK_EX)) < 0) {
      LOGE("Failed to lock file %s, errno=%d", fn.c_str(), errno);
    }
  }
  ~FileLock() { close(fd_); }

private:
  int fd_ = -1;
};

std::unordered_map<std::string, uint32_t> keys = {
    {"AccessToken", CLEAR_ON_MANAGER_START | DONT_LOG},
    {"AssistNowToken", PERSISTENT},
    {"AthenadPid", PERSISTENT},
    {"AthenadUploadQueue", PERSISTENT},
    {"CalibrationParams", PERSISTENT},
    {"CameraDebugExpGain", CLEAR_ON_MANAGER_START},
    {"CameraDebugExpTime", CLEAR_ON_MANAGER_START},
    {"CarBatteryCapacity", PERSISTENT},
    {"CarParams", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_ON},
    {"CarParamsCache", CLEAR_ON_MANAGER_START},
    {"CarParamsPersistent", PERSISTENT},
    {"CarVin", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_ON},
    {"CompletedTrainingVersion", PERSISTENT},
    {"ControlsReady", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_ON},
    {"CurrentRoute", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_ON},
    {"DashcamOverride", PERSISTENT},
    {"DisableLogging", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_ON},
    {"DisablePowerDown", PERSISTENT},
    {"ExperimentalMode", PERSISTENT},
    {"ExperimentalModeConfirmed", PERSISTENT},
    {"ExperimentalLongitudinalEnabled", PERSISTENT},
    {"DisableUpdates", PERSISTENT},
    {"DisengageOnAccelerator", PERSISTENT},
    {"DongleId", PERSISTENT},
    {"DoReboot", CLEAR_ON_MANAGER_START},
    {"DoShutdown", CLEAR_ON_MANAGER_START},
    {"DoUninstall", CLEAR_ON_MANAGER_START},
    {"FirmwareObdQueryDone", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_ON},
    {"ForcePowerDown", CLEAR_ON_MANAGER_START},
    {"GitBranch", PERSISTENT},
    {"GitCommit", PERSISTENT},
    {"GitDiff", PERSISTENT},
    {"GithubSshKeys", PERSISTENT},
    {"GithubUsername", PERSISTENT},
    {"GitRemote", PERSISTENT},
    {"GsmApn", PERSISTENT},
    {"GsmMetered", PERSISTENT},
    {"GsmRoaming", PERSISTENT},
    {"HardwareSerial", PERSISTENT},
    {"HasAcceptedTerms", PERSISTENT},
    {"IMEI", PERSISTENT},
    {"InstallDate", PERSISTENT},
    {"IsDriverViewEnabled", CLEAR_ON_MANAGER_START},
    {"IsEngaged", PERSISTENT},
    {"IsLdwEnabled", PERSISTENT},
    {"IsMetric", PERSISTENT},
    {"IsOffroad", CLEAR_ON_MANAGER_START},
    {"IsOnroad", PERSISTENT},
    {"IsRhdDetected", PERSISTENT},
    {"IsTakingSnapshot", CLEAR_ON_MANAGER_START},
    {"IsTestedBranch", CLEAR_ON_MANAGER_START},
    {"IsReleaseBranch", CLEAR_ON_MANAGER_START},
    {"IsUpdateAvailable", CLEAR_ON_MANAGER_START},
    {"JoystickDebugMode", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_OFF},
    {"LaikadEphemeris", PERSISTENT | DONT_LOG},
    {"LanguageSetting", PERSISTENT},
    {"LastAthenaPingTime", CLEAR_ON_MANAGER_START},
    {"LastGPSPosition", PERSISTENT},
    {"LastManagerExitReason", CLEAR_ON_MANAGER_START},
    {"LastPowerDropDetected", CLEAR_ON_MANAGER_START},
    {"LastSystemShutdown", CLEAR_ON_MANAGER_START},
    {"LastUpdateException", CLEAR_ON_MANAGER_START},
    {"LastUpdateTime", PERSISTENT},
    {"LiveParameters", PERSISTENT},
    {"LiveTorqueCarParams", PERSISTENT},
    {"LiveTorqueParameters", PERSISTENT | DONT_LOG},
    {"NavDestination", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_OFF},
    {"NavDestinationWaypoints", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_OFF},
    {"NavSettingTime24h", PERSISTENT},
    {"NavSettingLeftSide", PERSISTENT},
    {"NavdRender", PERSISTENT},
    {"ObdMultiplexingDisabled", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_ON},
    {"OpenpilotEnabledToggle", PERSISTENT},
    {"OsmLocal", PERSISTENT},
    {"OsmLocalDb", PERSISTENT},
    {"OsmLocationName", PERSISTENT},
    {"OsmLocationTitle", PERSISTENT},
    {"OsmLocationUrl", PERSISTENT},
    {"OsmDbUpdatesCheck", PERSISTENT},
    {"OsmWayTest", PERSISTENT},
    {"PandaHeartbeatLost", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_OFF},
    {"PandaSignatures", CLEAR_ON_MANAGER_START},
    {"Passive", PERSISTENT},
    {"PrimeType", PERSISTENT},
    {"RecordFront", PERSISTENT},
    {"RecordFrontLock", PERSISTENT},  // for the internal fleet
    {"ReplayControlsState", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_ON},
    {"ShouldDoUpdate", CLEAR_ON_MANAGER_START},
    {"ShowDebugUI", PERSISTENT},
    {"SnoozeUpdate", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_OFF},
    {"SpeedLimitControl", PERSISTENT},
    {"SpeedLimitPercOffset", PERSISTENT},
    {"SshEnabled", PERSISTENT},
    {"SubscriberInfo", PERSISTENT},
    {"TermsVersion", PERSISTENT},
    {"Timezone", PERSISTENT},
    {"TrainingVersion", PERSISTENT},
    {"TurnSpeedControl", PERSISTENT},
    {"TurnVisionControl", PERSISTENT},
    {"UbloxAvailable", PERSISTENT},
    {"UpdateAvailable", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_ON},
    {"UpdateFailedCount", CLEAR_ON_MANAGER_START},
    {"UpdaterState", CLEAR_ON_MANAGER_START},
    {"UpdaterFetchAvailable", CLEAR_ON_MANAGER_START},
    {"UpdaterTargetBranch", CLEAR_ON_MANAGER_START},
    {"UpdaterAvailableBranches", CLEAR_ON_MANAGER_START},
    {"UpdaterCurrentDescription", CLEAR_ON_MANAGER_START},
    {"UpdaterCurrentReleaseNotes", CLEAR_ON_MANAGER_START},
    {"UpdaterNewDescription", CLEAR_ON_MANAGER_START},
    {"UpdaterNewReleaseNotes", CLEAR_ON_MANAGER_START},
    {"Version", PERSISTENT},
    {"VisionRadarToggle", PERSISTENT},
    {"WideCameraOnly", PERSISTENT},
    {"ApiCache_Device", PERSISTENT},
    {"ApiCache_DriveStats", PERSISTENT},
    {"ApiCache_NavDestinations", PERSISTENT},
    {"ApiCache_Owner", PERSISTENT},
    {"Offroad_BadNvme", CLEAR_ON_MANAGER_START},
    {"Offroad_CarUnrecognized", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_ON},
    {"Offroad_ConnectivityNeeded", CLEAR_ON_MANAGER_START},
    {"Offroad_ConnectivityNeededPrompt", CLEAR_ON_MANAGER_START},
    {"Offroad_InvalidTime", CLEAR_ON_MANAGER_START},
    {"Offroad_IsTakingSnapshot", CLEAR_ON_MANAGER_START},
    {"Offroad_NeosUpdate", CLEAR_ON_MANAGER_START},
    {"Offroad_NoFirmware", CLEAR_ON_MANAGER_START | CLEAR_ON_IGNITION_ON},
    {"Offroad_StorageMissing", CLEAR_ON_MANAGER_START},
    {"Offroad_TemperatureTooHigh", CLEAR_ON_MANAGER_START},
    {"Offroad_UnofficialHardware", CLEAR_ON_MANAGER_START},
    {"Offroad_UpdateFailed", CLEAR_ON_MANAGER_START},
    // dp
    {"dp_api_custom", PERSISTENT},
    {"dp_api_custom_url", PERSISTENT},
    {"dp_atl", PERSISTENT},
    {"dp_locale", PERSISTENT},
    {"dp_jetson", PERSISTENT},
    {"dp_nav", PERSISTENT},
    {"dp_otisserv", PERSISTENT},
    {"dp_nav_mapbox_token_pk", PERSISTENT},
    {"dp_nav_mapbox_token_sk", PERSISTENT},
    {"dp_nav_full_screen", PERSISTENT},
    {"dp_nav_gmap_enable", PERSISTENT},
    {"dp_nav_gmap_key", PERSISTENT},
    {"dp_nav_amap_enable", PERSISTENT},
    {"dp_nav_amap_key", PERSISTENT},
    {"dp_nav_amap_key_2", PERSISTENT},
    {"dp_nav_style_day", PERSISTENT},
    {"dp_nav_style_night", PERSISTENT},
    {"dp_gpxd", PERSISTENT},
    {"dp_car_assigned", PERSISTENT},
    {"dp_car_list", PERSISTENT},
    {"dp_last_modified", PERSISTENT},
    {"dp_lateral_mode", PERSISTENT},
    {"dp_lc_min_mph", PERSISTENT},
    {"dp_lc_auto_min_mph", PERSISTENT},
    {"dp_lc_auto_delay", PERSISTENT},
    {"dp_lateral_lanelines", PERSISTENT},
    {"dp_lateral_camera_offset", PERSISTENT},
    {"dp_lateral_path_offset", PERSISTENT},
    {"dp_quiet_drive", PERSISTENT},
    {"dp_ui_top", PERSISTENT},
    {"dp_ui_side", PERSISTENT},
    {"dp_ui_brightness", PERSISTENT},
    {"dp_ui_display_mode", PERSISTENT},
    {"dp_ui_speed", PERSISTENT},
    {"dp_ui_event", PERSISTENT},
    {"dp_ui_face", PERSISTENT},
    {"dp_ui_lead_info", PERSISTENT},
    {"dp_ui_laneline", PERSISTENT},
    {"dp_ui_chevron", PERSISTENT},
    {"dp_ui_dm_cam", PERSISTENT},
    {"dp_toyota_sng", PERSISTENT},
    {"dp_accel_profile_ctrl", PERSISTENT},
    {"dp_accel_profile", PERSISTENT},
    {"dp_toyota_ap_btn_link", PERSISTENT},
    {"dp_toyota_cruise_override", PERSISTENT},
    {"dp_toyota_cruise_override_speed", PERSISTENT},
    {"dp_toyota_auto_lock", PERSISTENT},
    {"dp_toyota_auto_unlock", PERSISTENT},
    {"dp_toyota_fp_btn_link", PERSISTENT},
    {"dp_mapd", PERSISTENT},
    {"dp_local_db", PERSISTENT},
    {"dp_dashcamd", PERSISTENT},
    {"dp_auto_shutdown", PERSISTENT},
    {"dp_auto_shutdown_in", PERSISTENT},
    {"dp_mazda_steer_alert", PERSISTENT},
    {"dp_mazda_dashcam_bypass", PERSISTENT},
    {"dp_mazda_ti", PERSISTENT},
    {"dp_hotspot_on_boot", PERSISTENT},
    {"dp_honda_eps_mod", PERSISTENT},
    {"dp_dm", PERSISTENT},
    {"dp_speed_check", PERSISTENT},
    {"dp_temp_check", PERSISTENT},
    {"dp_following_profile_ctrl", PERSISTENT},
    {"dp_following_profile", PERSISTENT},
    {"dp_lateral_tune", PERSISTENT},
    {"dp_lateral_torque_live_tune", PERSISTENT},
    {"dp_toyota_rav4_tss2_tune", PERSISTENT},
    {"dp_toyota_prius_bad_angle_tune", PERSISTENT},
    {"dp_e2e_conditional", PERSISTENT},
    {"dp_e2e_conditional_adapt_fp", PERSISTENT},
    {"dp_e2e_conditional_adapt_ap", PERSISTENT},
    {"dp_e2e_conditional_voacc", PERSISTENT},
    {"dp_toyota_zss", PERSISTENT},
    {"dp_toyota_change5speed", PERSISTENT},
    {"dp_lateral_version", PERSISTENT},
    {"dp_lateral_steer_rate_cost", PERSISTENT},
};

} // namespace


Params::Params(const std::string &path) {
  prefix = "/" + util::getenv("OPENPILOT_PREFIX", "d");
  params_path = ensure_params_path(prefix, path);
}

std::vector<std::string> Params::allKeys() const {
  std::vector<std::string> ret;
  for (auto &p : keys) {
    ret.push_back(p.first);
  }
  return ret;
}

bool Params::checkKey(const std::string &key) {
  return keys.find(key) != keys.end();
}

ParamKeyType Params::getKeyType(const std::string &key) {
  return static_cast<ParamKeyType>(keys[key]);
}

int Params::put(const char* key, const char* value, size_t value_size) {
  // Information about safely and atomically writing a file: https://lwn.net/Articles/457667/
  // 1) Create temp file
  // 2) Write data to temp file
  // 3) fsync() the temp file
  // 4) rename the temp file to the real name
  // 5) fsync() the containing directory
  std::string tmp_path = params_path + "/.tmp_value_XXXXXX";
  int tmp_fd = mkstemp((char*)tmp_path.c_str());
  if (tmp_fd < 0) return -1;

  int result = -1;
  do {
    // Write value to temp.
    ssize_t bytes_written = HANDLE_EINTR(write(tmp_fd, value, value_size));
    if (bytes_written < 0 || (size_t)bytes_written != value_size) {
      result = -20;
      break;
    }

    // fsync to force persist the changes.
    if ((result = fsync(tmp_fd)) < 0) break;

    FileLock file_lock(params_path + "/.lock");

    // Move temp into place.
    if ((result = rename(tmp_path.c_str(), getParamPath(key).c_str())) < 0) break;

    // fsync parent directory
    result = fsync_dir(getParamPath());
  } while (false);

  close(tmp_fd);
  ::unlink(tmp_path.c_str());
  return result;
}

int Params::remove(const std::string &key) {
  FileLock file_lock(params_path + "/.lock");
  int result = unlink(getParamPath(key).c_str());
  if (result != 0) {
    return result;
  }
  return fsync_dir(getParamPath());
}

std::string Params::get(const std::string &key, bool block) {
  if (!block) {
    return util::read_file(getParamPath(key));
  } else {
    // blocking read until successful
    params_do_exit = 0;
    void (*prev_handler_sigint)(int) = std::signal(SIGINT, params_sig_handler);
    void (*prev_handler_sigterm)(int) = std::signal(SIGTERM, params_sig_handler);

    std::string value;
    while (!params_do_exit) {
      if (value = util::read_file(getParamPath(key)); !value.empty()) {
        break;
      }
      util::sleep_for(100);  // 0.1 s
    }

    std::signal(SIGINT, prev_handler_sigint);
    std::signal(SIGTERM, prev_handler_sigterm);
    return value;
  }
}

std::map<std::string, std::string> Params::readAll() {
  FileLock file_lock(params_path + "/.lock");
  return util::read_files_in_dir(getParamPath());
}

void Params::clearAll(ParamKeyType key_type) {
  FileLock file_lock(params_path + "/.lock");

  if (key_type == ALL) {
    util::remove_files_in_dir(getParamPath());
  } else {
    for (auto &[key, type] : keys) {
      if (type & key_type) {
        unlink(getParamPath(key).c_str());
      }
    }
  }

  fsync_dir(getParamPath());
}
