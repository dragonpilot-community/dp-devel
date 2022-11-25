from cereal import car, log
import cereal.messaging as messaging
from common.conversions import Conversions as CV
from common.params import Params
from opendbc.can.packer import CANPacker
from selfdrive.car import apply_std_steer_torque_limits
from selfdrive.car.subaru import subarucan
from selfdrive.car.subaru.values import DBC, GLOBAL_GEN2, PREGLOBAL_CARS, CarControllerParams


class CarController:
  def __init__(self, dbc_name, CP, VM):
    self.CP = CP
    self.apply_steer_last = 0
    self.frame = 0
    
    self.sng_acc_resume = False
    self.sng_acc_resume_cnt = -1
    self.manual_hold = False
    self.prev_cruise_state = 0
    self.prev_close_distance = 0

    self.es_lkas_cnt = -1
    self.es_distance_cnt = -1
    self.es_dashstatus_cnt = -1
    self.cruise_button_prev = 0
    self.last_cancel_frame = 0
    self.throttle_cnt = -1
    self.brake_pedal_cnt = -1
    
    self.sm = messaging.SubMaster(['liveMapData', 'controlsState', 'longitudinalPlan'])
    self.param_s = Params()
    self.speed_limit_control_enabled = self.param_s.get_bool("SpeedLimitControl")
    self.map_turn_speed_control_enabled = self.param_s.get_bool("TurnSpeedControl")
    self.is_metric = self.param_s.get_bool("IsMetric")
    self.speed_limit = 0
    self.turn_limit = 0
    self.prev_speed_limit = 0
    self.prev_turn_limit = 0
    self.adjust_speed = False
    self.speed_lim_change = False
    #self.speed_limit_perc_offset = self.param_s.get_bool("SpeedLimitPercOffset")
    #self.speed_limit_value_offset = int(self.param_s.get("SpeedLimitValueOffset"))
    
    self.cruise_buttons_cnt = -1
    
    self.dn_button_press = False
    self.dn_button_press_cnt = -1
    
    self.up_button_press = False
    self.up_button_press_cnt = -1
    
    self.p = CarControllerParams(CP)
    self.packer = CANPacker(DBC[CP.carFingerprint]['pt'])
    
    self.splim_min = self.p.SPDL_MIN * (1 if self.is_metric else CV.KPH_TO_MPH)
    self.splim_max = self.p.SPDL_MAX * (1 if self.is_metric else CV.KPH_TO_MPH)
    self.splim_del = self.p.SPDL_DLT * (1 if self.is_metric else CV.KPH_TO_MPH)
    

  def update(self, CC, CS):
  

  
    actuators = CC.actuators
    hud_control = CC.hudControl
    pcm_cancel_cmd = CC.cruiseControl.cancel

    can_sends = []
    
    
    # *** Set the LKAS mode to display at RPM - JPR ***
    if (((self.frame + 15) % 30) == 0) or (self.frame == 0):
      CS.speed_limit = CS.lkas_mode  #Set the indicator correctly
    # *** End Set the LKAS mode ***

    # *** steering ***
    if (self.frame % self.p.STEER_STEP) == 0:

      apply_steer = int(round(actuators.steer * self.p.STEER_MAX))

      # limits due to driver torque

      new_steer = int(round(apply_steer))
      apply_steer = apply_std_steer_torque_limits(new_steer, self.apply_steer_last, CS.out.steeringTorque, self.p)

      if not CC.latActive:
        apply_steer = 0

      if not (CS.lkas_mode == 3):
        if not (CS.cruise_lkas_state == 2):
          apply_steer = 0
      
      if self.CP.carFingerprint in PREGLOBAL_CARS:
        can_sends.append(subarucan.create_preglobal_steering_control(self.packer, apply_steer))
      else:
        can_sends.append(subarucan.create_steering_control(self.packer, apply_steer))

      self.apply_steer_last = apply_steer

    


    # *** alerts and pcm cancel ***

    if self.CP.carFingerprint in PREGLOBAL_CARS:
      if self.es_distance_cnt != CS.es_distance_msg["COUNTER"]:
        # 1 = main, 2 = set shallow, 3 = set deep, 4 = resume shallow, 5 = resume deep
        # disengage ACC when OP is disengaged
        if pcm_cancel_cmd:
          cruise_button = 1
        # turn main on if off and past start-up state
        elif not CS.out.cruiseState.available and CS.ready:
          cruise_button = 1
        else:
          cruise_button = CS.cruise_button

        # unstick previous mocked button press
        if cruise_button == 1 and self.cruise_button_prev == 1:
          cruise_button = 0
        self.cruise_button_prev = cruise_button

        can_sends.append(subarucan.create_preglobal_es_distance(self.packer, cruise_button, CS.es_distance_msg))
        self.es_distance_cnt = CS.es_distance_msg["COUNTER"]

    else:
    
      # *** speed control - check every half second - km/h - JPR ***
      cspeed_dn_cmd = False
      cspeed_up_cmd = False
    
      #check every two seconds for speed change
      if ((self.frame % 200) == 0):
    
    
        #set initial speed limit setup
        self.sm.update(0)
        self.speed_limit_control_enabled = self.param_s.get_bool("SpeedLimitControl")
        if self.speed_limit_control_enabled:
        
          #self.speed_limit_perc_offset = self.param_s.get_bool("SpeedLimitPercOffset")
          #self.speed_limit_value_offset = int(self.param_s.get("SpeedLimitValueOffset"))
          self.get_speed_limit()  
        #else:
        #  CS.speed_limit = 0
        #end initial speed limit setup
    
        #check to see if the speed limit has changed
        if (self.speed_limit != self.prev_speed_limit) and (self.speed_limit_control_enabled):
          self.speed_lim_change = True     
    
      #check every four seconds for turn speed change
      if (((self.frame+100) % 400) == 0):
        #check to see if the turn limit has changed  
        self.map_turn_speed_control_enabled = self.param_s.get_bool("TurnSpeedControl")
      
        if self.map_turn_speed_control_enabled:
          self.get_turn_limit()
        else:
          self.turn_limit = self.speed_limit
      
        if (self.turn_limit != self.prev_turn_limit) and (self.speed_limit_control_enabled) and (self.map_turn_speed_control_enabled):
          self.speed_lim_change = True
    
      #check every two seconds for speed change done   
    
      #adapt the speed appropriatly
      if (self.speed_lim_change) and (((self.frame+50) % 100) == 0):
      
        new_speedlim = (int(((self.speed_limit * (CV.MS_TO_KPH if self.is_metric else CV.MS_TO_MPH)) + 2.5) / 5) * 5)
        new_tunrlim  = (int(((self.turn_limit * (CV.MS_TO_KPH if self.is_metric else CV.MS_TO_MPH)) + 2.5) / 5) * 5)
          
        cur_setspeed = int(((CS.out.cruiseState.speed * (CV.MS_TO_KPH if self.is_metric else CV.MS_TO_MPH)) + 2.5) / 5) * 5
      
        new_cspeed = new_speedlim
      
        if new_tunrlim == 0:
          new_tunrlim = new_speedlim
      
        if min(new_speedlim, new_tunrlim) > 0:
          CS.speed_limit = min(new_speedlim, new_tunrlim)
          
        #if speed limit is undefined, and turn limit is less than set speed limit
        if ((new_speedlim == 0) and (new_tunrlim > self.splim_min) and (new_tunrlim < cur_setspeed)):
          new_cspeed = new_tunrlim
          self.adjust_speed = True
            
        #if speed limit is defined, and turn limit is less than set speed limit
        if ((new_speedlim > self.splim_min) and (new_tunrlim > self.splim_min) and (new_tunrlim < new_speedlim)):
          new_cspeed = new_tunrlim
          self.adjust_speed = True
            
        #if the speed limit is defined and the turn limit is larger
        if ((new_speedlim > self.splim_min) and (new_tunrlim > self.splim_min) and (new_tunrlim > new_speedlim)):
          new_cspeed = new_speedlim
            
        #if the speed limit is defined and the turn limit is not defined
        if ((new_speedlim > self.splim_min) and (new_tunrlim == 0) and (new_speedlim < cur_setspeed)):
          new_cspeed = new_speedlim
          
        #CS.speed_limit = new_cspeed
              
        if ((CS.cruise_state == 0) and (CS.out.cruiseState.enabled) and (CS.speed_limit!=0)):
                          
          new_cspeed = new_cspeed + 5
            
          if ((abs(cur_setspeed - new_cspeed) > self.splim_del) and (CS.speed_limit != 0) and (CS.speed_limit > self.splim_min)):
            self.adjust_speed = True
              
          if ((cur_setspeed == new_cspeed) or (CS.speed_limit == 0)):
            self.adjust_speed = False
            self.speed_lim_change = False
            
          if ((cur_setspeed != new_cspeed) and (CS.speed_limit != 0) and (self.adjust_speed)):
              
            #Check down first - but to a miminum of 30 kph
            if ((new_cspeed > self.splim_min) and (new_cspeed < cur_setspeed)):
              self.dn_button_press = True  #press the down button
                
            else:
              #Check up more than 30 kph, but to a max of 131 kph
              if ((new_cspeed < self.splim_max) and (new_cspeed > (cur_setspeed + 0))):
                self.up_button_press = True  #press the up button

      #set previous speed limit every two seconds
      if ((self.frame % 200) == 0):
        if self.speed_limit_control_enabled and (self.speed_limit != 0):
          self.prev_speed_limit = self.speed_limit
    
      if (((self.frame+100) % 400) == 0):    
        if self.map_turn_speed_control_enabled and self.speed_limit_control_enabled and (self.turn_limit != 0):  
          self.prev_turn_limit = self.turn_limit
      #set previous speed limit every two seconds done
    

      # *** speed control - press the up or down buttons***

      #press the down button if required for 5 frames
      if self.dn_button_press:
        if ((self.dn_button_press_cnt < 5) and (self.up_button_press == False)):
          cspeed_dn_cmd = True
          self.dn_button_press_cnt += 1
        else:
          self.dn_button_press = False
          self.dn_button_press_cnt = -1
         
      #press the up button if required for 5 frames
      if self.up_button_press:
        if ((self.up_button_press_cnt < 5) and (self.dn_button_press == False)):
          cspeed_up_cmd = True
          self.up_button_press_cnt += 1
        else:
          self.up_button_press = False
          self.up_button_press_cnt = -1
   

   

      # *** stop and go - MLP Global platform***
      throttle_cmd = False
      speed_cmd = False
    
   

      # Record manual hold set while in standstill and no car in front
      if CS.out.standstill and self.prev_cruise_state == 1 and CS.cruise_state == 3 and CS.car_follow == 0:
        self.manual_hold = True
      # Cancel manual hold when car starts moving
      if not CS.out.standstill:
        self.manual_hold = False
      if (CS.out.cruiseState.enabled                                            # ACC active
          and not self.manual_hold
          and CS.car_follow == 1                             # lead car
          and CS.cruise_state == 3                           # ACC HOLD (only with EPB)
          and CS.out.standstill                              # must be standing still
          and CS.close_distance > self.p.ACC_MIN_DIST        # acc resume min trigger threshold (m)
          and CS.close_distance < self.p.ACC_MAX_DIST        # acc resume max trigger threshold (m)
          and CS.close_distance > self.prev_close_distance): # distance with lead car is increasing
        self.sng_acc_resume = True       #JP test
        #self.up_button_press = True  #press the up button  #JP test
      self.prev_cruise_state = CS.cruise_state

      if self.sng_acc_resume:  #JP test
        if self.sng_acc_resume_cnt < 5:
          throttle_cmd = True
          self.sng_acc_resume_cnt += 1
        else:
          self.sng_acc_resume = False
          self.sng_acc_resume_cnt = -1

      # Cancel ACC if stopped, brake pressed and not stopped behind another car
      if CS.out.cruiseState.enabled and CS.out.brakePressed and CS.car_follow == 0 and CS.out.standstill:
        pcm_cancel_cmd = True

      self.prev_close_distance = CS.close_distance
    
      # *** alerts and pcm cancel GLOBAL ***
    
      # if pcm_cancel_cmd and (self.frame - self.last_cancel_frame) > 0.2:
        # bus = 1 if self.CP.carFingerprint in GLOBAL_GEN2 else 0
        # can_sends.append(subarucan.create_es_distance(self.packer, CS.es_distance_msg, bus, pcm_cancel_cmd))
        # self.last_cancel_frame = self.frame
      
      if self.es_distance_cnt != CS.es_distance_msg["COUNTER"]:
        can_sends.append(subarucan.create_es_distance(self.packer, CS.es_distance_msg, 0 , pcm_cancel_cmd, cspeed_dn_cmd, cspeed_up_cmd))
        self.es_distance_cnt = CS.es_distance_msg["COUNTER"]

      if self.es_dashstatus_cnt != CS.es_dashstatus_msg["COUNTER"]:
        can_sends.append(subarucan.create_es_dashstatus(CS.lkas_mode, self.packer, CS.es_dashstatus_msg))
        self.es_dashstatus_cnt = CS.es_dashstatus_msg["COUNTER"]

      if self.es_lkas_cnt != CS.es_lkas_msg["COUNTER"]:
        can_sends.append(subarucan.create_es_lkas(CS.lkas_mode, CS.lkasm_change, self.sng_acc_resume, self.packer, CS.es_lkas_msg, CC.enabled, hud_control.visualAlert,
                                                  hud_control.leftLaneVisible, hud_control.rightLaneVisible,
                                                  hud_control.leftLaneDepart, hud_control.rightLaneDepart))
        self.es_lkas_cnt = CS.es_lkas_msg["COUNTER"]
        
      if self.cruise_buttons_cnt != CS.sw_cruise_buttons_msg["COUNTER"]:
        can_sends.append(subarucan.create_cruise_buttons(self.packer, CS.sw_cruise_buttons_msg, cspeed_dn_cmd, cspeed_up_cmd))
        self.cruise_buttons_cnt = CS.sw_cruise_buttons_msg["COUNTER"]
      
      if self.throttle_cnt != CS.throttle_msg["COUNTER"]:
        can_sends.append(subarucan.create_throttle(self.packer, CS.throttle_msg, throttle_cmd))
        self.throttle_cnt = CS.throttle_msg["COUNTER"]
        
      if self.brake_pedal_cnt != CS.brake_pedal_msg["COUNTER"]:
        can_sends.append(subarucan.create_brake_pedal(self.packer, CS.brake_pedal_msg, speed_cmd, pcm_cancel_cmd))
        self.brake_pedal_cnt = CS.brake_pedal_msg["COUNTER"]

    new_actuators = actuators.copy()
    new_actuators.steer = self.apply_steer_last / self.p.STEER_MAX

    self.frame += 1
    return new_actuators, can_sends
    
  def get_speed_limit(self):
    self.speed_limit = float(self.sm['longitudinalPlan'].speedLimit if self.sm['longitudinalPlan'].speedLimit is not None else 0.0)
    return self.speed_limit
    
  def get_turn_limit(self):
    self.turn_limit = float(self.sm['longitudinalPlan'].turnSpeed if self.sm['longitudinalPlan'].turnSpeed is not None else 0.0) 
    return self.turn_limit

