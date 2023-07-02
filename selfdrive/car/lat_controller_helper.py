'''

dp - we create a separate controller helper to restore PID/LQR steering tune.
'''

from selfdrive.car.tunes import set_lat_tune, LatTunes
from selfdrive.car.toyota.values import CAR as TOYOTA
from selfdrive.car.hyundai.values import CAR as HYUNDAI
from selfdrive.car.volkswagen.values import CAR as VW
from selfdrive.car.subaru.values import CAR as SUBARU

from common.params import Params

def configure_pid_tune(candidate, tune):
  # toyota
  if candidate == TOYOTA.PRIUS:
    # indi only
    pass
  elif candidate == TOYOTA.PRIUS_V:
    # lqr only
    pass
  elif candidate in (TOYOTA.RAV4, TOYOTA.RAV4H):
    # lqr only
    pass
  elif candidate == TOYOTA.COROLLA:
    set_lat_tune(tune, LatTunes.PID_A)
  elif candidate in (TOYOTA.LEXUS_RX, TOYOTA.LEXUS_RXH, TOYOTA.LEXUS_RX_TSS2, TOYOTA.LEXUS_RXH_TSS2):
    set_lat_tune(tune, LatTunes.PID_C)
  elif candidate in (TOYOTA.CHR, TOYOTA.CHRH, TOYOTA.CHR_TSS2):
    set_lat_tune(tune, LatTunes.PID_F)
  elif candidate in (TOYOTA.CAMRY, TOYOTA.CAMRYH, TOYOTA.CAMRY_TSS2, TOYOTA.CAMRYH_TSS2):
    if candidate not in (TOYOTA.CAMRY_TSS2, TOYOTA.CAMRYH_TSS2):
      set_lat_tune(tune, LatTunes.PID_C)
  elif candidate in (TOYOTA.HIGHLANDER, TOYOTA.HIGHLANDERH, TOYOTA.HIGHLANDER_TSS2, TOYOTA.HIGHLANDERH_TSS2):
    set_lat_tune(tune, LatTunes.PID_G)
  elif candidate in (TOYOTA.AVALON, TOYOTA.AVALON_2019, TOYOTA.AVALONH_2019, TOYOTA.AVALON_TSS2, TOYOTA.AVALONH_TSS2):
    set_lat_tune(tune, LatTunes.PID_H)
  elif candidate in (TOYOTA.RAV4_TSS2, TOYOTA.RAV4_TSS2_2022, TOYOTA.RAV4H_TSS2, TOYOTA.RAV4H_TSS2_2022):
    # 2019+ RAV4 TSS2 uses two different steering racks and specific tuning seems to be necessary.
    if Params().get_bool("dp_toyota_rav4_tss2_tune"):
      set_lat_tune(tune, LatTunes.PID_I)
    else:
      set_lat_tune(tune, LatTunes.PID_D)
  elif candidate in (TOYOTA.COROLLA_TSS2, TOYOTA.COROLLAH_TSS2):
    set_lat_tune(tune, LatTunes.PID_D)
  elif candidate in (TOYOTA.LEXUS_ES_TSS2, TOYOTA.LEXUS_ESH_TSS2, TOYOTA.LEXUS_ESH):
    set_lat_tune(tune, LatTunes.PID_D)
  elif candidate == TOYOTA.SIENNA:
    set_lat_tune(tune, LatTunes.PID_J)
  elif candidate in (TOYOTA.LEXUS_IS, TOYOTA.LEXUS_RC):
    set_lat_tune(tune, LatTunes.PID_L)
  elif candidate == TOYOTA.LEXUS_CTH:
    set_lat_tune(tune, LatTunes.PID_M)
  elif candidate in (TOYOTA.LEXUS_NX, TOYOTA.LEXUS_NXH, TOYOTA.LEXUS_NX_TSS2, TOYOTA.LEXUS_NXH_TSS2):
    set_lat_tune(tune, LatTunes.PID_C)
  elif candidate == TOYOTA.PRIUS_TSS2:
    set_lat_tune(tune, LatTunes.PID_N)
  elif candidate == TOYOTA.MIRAI:
    set_lat_tune(tune, LatTunes.PID_C)
  elif candidate in (TOYOTA.ALPHARD_TSS2, TOYOTA.ALPHARDH_TSS2):
    set_lat_tune(tune, LatTunes.PID_J)

  # hyundai
  elif candidate in (HYUNDAI.SANTA_FE, HYUNDAI.SANTA_FE_2022, HYUNDAI.SANTA_FE_HEV_2022, HYUNDAI.SANTA_FE_PHEV_2022):
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_D)
  elif candidate in (HYUNDAI.SONATA, HYUNDAI.SONATA_HYBRID):
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_A)
  elif candidate == HYUNDAI.SONATA_LF:
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_A)
  elif candidate == HYUNDAI.PALISADE:
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_C)
  elif candidate == HYUNDAI.ELANTRA:
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_B)
  elif candidate == HYUNDAI.ELANTRA_2021:
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_A)
  elif candidate == HYUNDAI.ELANTRA_HEV_2021:
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_A)
  elif candidate == HYUNDAI.HYUNDAI_GENESIS:
    # indi only
    pass
  elif candidate in (HYUNDAI.KONA, HYUNDAI.KONA_EV, HYUNDAI.KONA_HEV):
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_A)
  elif candidate in (HYUNDAI.IONIQ, HYUNDAI.IONIQ_EV_LTD, HYUNDAI.IONIQ_EV_2020, HYUNDAI.IONIQ_PHEV, HYUNDAI.IONIQ_HEV_2022):
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_B)
  elif candidate == HYUNDAI.IONIQ_PHEV_2019:
    # indi only
    pass
  elif candidate == HYUNDAI.VELOSTER:
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_A)

  # Kia
  elif candidate == HYUNDAI.KIA_SORENTO:
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_A)
  elif candidate in (HYUNDAI.KIA_NIRO_EV, HYUNDAI.KIA_NIRO_HEV_2021):
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_B)
  elif candidate == HYUNDAI.KIA_SELTOS:
    # indi only
    pass
  elif candidate == HYUNDAI.KIA_OPTIMA_H:
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_A)
  elif candidate == HYUNDAI.KIA_STINGER:
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_A)
  elif candidate == HYUNDAI.KIA_FORTE:
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_A)
  elif candidate == HYUNDAI.KIA_CEED:
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_A)
  elif candidate == HYUNDAI.KIA_K5_2021:
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_A)

  # Genesis
  elif candidate == HYUNDAI.GENESIS_G70:
    # indi only
    pass
  elif candidate == HYUNDAI.GENESIS_G70_2020:
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_E)
  elif candidate == HYUNDAI.GENESIS_G80:
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_F)
  elif candidate == HYUNDAI.GENESIS_G90:
    set_lat_tune(tune, LatTunes.PID_HYUNDAI_G)

  # VW
  elif candidate == VW.ARTEON_MK1:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.ATLAS_MK1:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.GOLF_MK7:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.JETTA_MK7:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.PASSAT_MK8:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.PASSAT_NMS:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.POLO_MK6:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.SHARAN_MK2:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.TAOS_MK1:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.TCROSS_MK1:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.TIGUAN_MK2:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.TOURAN_MK2:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.TRANSPORTER_T61:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.TROC_MK1:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.AUDI_A3_MK3:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.AUDI_Q2_MK1:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.AUDI_Q3_MK2:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.SEAT_ATECA_MK1:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.SEAT_LEON_MK3:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.SKODA_KAMIQ_MK1:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.SKODA_KAROQ_MK1:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.SKODA_KODIAQ_MK1:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.SKODA_OCTAVIA_MK3:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.SKODA_SCALA_MK1:
    set_lat_tune(tune, LatTunes.PID_VW)
  elif candidate == VW.SKODA_SUPERB_MK3:
    set_lat_tune(tune, LatTunes.PID_VW)

  # subaru
  elif candidate == SUBARU.ASCENT:
    set_lat_tune(tune, LatTunes.PID_SUBARU_A)
  elif candidate == SUBARU.IMPREZA:
    set_lat_tune(tune, LatTunes.PID_SUBARU_B)
  elif candidate == SUBARU.IMPREZA_2020:
    set_lat_tune(tune, LatTunes.PID_SUBARU_C)
  elif candidate == SUBARU.FORESTER:
    set_lat_tune(tune, LatTunes.PID_SUBARU_D)
  elif candidate in (SUBARU.OUTBACK, SUBARU.LEGACY):
    # torque only
    pass
  elif candidate in (SUBARU.FORESTER_PREGLOBAL, SUBARU.OUTBACK_PREGLOBAL_2018):
    set_lat_tune(tune, LatTunes.PID_SUBARU_E)
  elif candidate == SUBARU.LEGACY_PREGLOBAL:
    set_lat_tune(tune, LatTunes.PID_SUBARU_F)
  elif candidate == SUBARU.OUTBACK_PREGLOBAL:
    set_lat_tune(tune, LatTunes.PID_SUBARU_E)


'''
from RAV4
'''
def configure_lqr_tune(candidate, tune):
  tune.init('lqr')
  tune.lqr.scale = 1500.0
  tune.lqr.ki = 0.05

  tune.lqr.a = [0., 1., -0.22619643, 1.21822268]
  tune.lqr.b = [-1.92006585e-04, 3.95603032e-05]
  tune.lqr.c = [1., 0.]
  tune.lqr.k = [-110.73572306, 451.22718255]
  tune.lqr.l = [0.3233671, 0.3185757]
  tune.lqr.dcGain = 0.002237852961363602

# '''
# directly copy from CarInterface.configure_torque_tune
# '''
# def config_torque_tune(candidate, tune, steering_angle_deadzone_deg=0.0, use_steering_angle=True):
#   try:
#     params = get_torque_params(candidate)
#
#     tune.init('torque')
#     tune.torque.useSteeringAngle = use_steering_angle
#     tune.torque.kp = 1.0
#     tune.torque.kf = 1.0
#     tune.torque.ki = 0.1
#     tune.torque.friction = params['FRICTION']
#     tune.torque.latAccelFactor = params['LAT_ACCEL_FACTOR']
#     tune.torque.latAccelOffset = 0.0
#     tune.torque.steeringAngleDeadzoneDeg = steering_angle_deadzone_deg
#   except:
#     pass
