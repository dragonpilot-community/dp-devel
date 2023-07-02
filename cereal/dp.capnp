using Cxx = import "./include/c++.capnp";
$Cxx.namespace("cereal");

@0xbfa7e645486440c7;

# dp
struct DragonConf {
  dpAtl @0 :UInt8;
  dpLocale @1 :Text;
  dpLateralMode @2 :UInt8;
  dpLcMinMph @3 :UInt8;
  dpLcAutoMinMph @4 :UInt8;
  dpLcAutoDelay @5 :Float32;
  dpLateralLanelines @6 :Bool;
  dpLateralCameraOffset @7 :Int8;
  dpLateralPathOffset @8 :Int8;
  dpLateralRoadEdgeDetected @9 :Bool;
  dpIpAddr @10 :Text;
  dpUiTop @11 :Bool;
  dpUiSide @12 :Bool;
  dpUiBrightness @13 :UInt8;
  dpUiDisplayMode @14 :UInt8;
  dpUiSpeed @15 :Bool;
  dpUiEvent @16 :Bool;
  dpUiFace @17 :Bool;
  dpUiLeadInfo @18 :Bool;
  dpUiLaneline @19 :Bool;
  dpUiChevron @20 :Bool;
  dpUiDmCam @21 :Bool;
  dpUiRainbow @22 :Bool;
  dpToyotaSng @23 :Bool;
  dpAccelProfileCtrl @24 :Bool;
  dpAccelProfile @25 :UInt8;
  dpToyotaCruiseOverride @26 :Bool;
  dpToyotaCruiseOverrideSpeed @27 :UInt8;
  dpToyotaAutoLock @28 :Bool;
  dpToyotaAutoUnlock @29 :Bool;
  dpToyotaDebugBsm @30 :Bool;
  dpMapd @31 :Bool;
  dpLocalDb @32 :Bool;
  dpDashcamd @33 :Bool;
  dpMazdaSteerAlert @34 :Bool;
  dpSpeedCheck @35 :Bool;
  dpFollowingProfileCtrl @36 :Bool;
  dpFollowingProfile @37 :UInt8;
  dpLateralAlt @38 :Bool;
  dpLateralAltSpeed @39 :UInt8;
  dpLateralAltCtrl @40 :UInt8;
  dpLateralAltLanelines @41 :Bool;
  dpLateralAltCameraOffset @42 :Int8;
  dpLateralAltPathOffset @43 :Int8;
  dpE2EConditional @44 :Bool;
  dpE2EConditionalAdaptFp @45 :Bool;
  dpE2EConditionalAdaptAp @46 :Bool;
  dpE2EConditionalVoacc @47 :Bool;
  dpLongLeadMovingAlert @48 :Int8;
  dpLateralLcManual @49 :Bool;
}
