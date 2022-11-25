import copy
from cereal import car

VisualAlert = car.CarControl.HUDControl.VisualAlert

def create_steering_control(packer, apply_steer):
  values = {
    "LKAS_Output": apply_steer,
    "LKAS_Request": 1 if apply_steer != 0 else 0,
    "SET_1": 1
  }
  return packer.make_can_msg("ES_LKAS", 0, values)

def create_steering_status(packer):
  return packer.make_can_msg("ES_LKAS_State", 0, {})

def create_es_distance(packer, es_distance_msg, bus, pcm_cancel_cmd, cspeed_dn_cmd, cspeed_up_cmd):
  values = copy.copy(es_distance_msg)
  #values["COUNTER"] = (values["COUNTER"] + 1) % 0x10
  if pcm_cancel_cmd:
    values["Cruise_Cancel"] = 1
    
  if ((cspeed_dn_cmd == True) and (cspeed_up_cmd == False)):
    values["Cruise_Set"] = 1    #JP Test
    
  if ((cspeed_up_cmd == True) and (cspeed_dn_cmd == False)):
    values["Cruise_Resume"] = 1  
  
  return packer.make_can_msg("ES_Distance", bus, values)

def create_es_lkas(lkas_mode, mchange, sng_acc_resume, packer, es_lkas_msg, enabled, visual_alert, left_line, right_line, left_lane_depart, right_lane_depart):

  values = copy.copy(es_lkas_msg)

  if (lkas_mode == 3):
  
    # Filter the stock LKAS "Keep hands on wheel" alert
    if values["LKAS_Alert_Msg"] == 1:
      values["LKAS_Alert_Msg"] = 0

    # Filter the stock LKAS sending an audible alert when it turns off LKAS
    if values["LKAS_Alert"] == 27:
      values["LKAS_Alert"] = 0

    # Filter the stock LKAS sending an audible alert when "Keep hands on wheel" alert is active (2020+ models)
    if values["LKAS_Alert"] == 28 and values["LKAS_Alert_Msg"] == 7:
      values["LKAS_Alert"] = 0

    # Filter the stock LKAS sending an audible alert when "Keep hands on wheel OFF" alert is active (2020+ models)
    if values["LKAS_Alert"] == 30:
      values["LKAS_Alert"] = 0

    # Filter the stock LKAS sending "Keep hands on wheel OFF" alert (2020+ models)
    if values["LKAS_Alert_Msg"] == 7:
      values["LKAS_Alert_Msg"] = 0

    # Show Keep hands on wheel alert for openpilot steerRequired alert
    if visual_alert == VisualAlert.steerRequired:
      values["LKAS_Alert_Msg"] = 1

    # Ensure we don't overwrite potentially more important alerts from stock (e.g. FCW)
    if visual_alert == VisualAlert.ldw and values["LKAS_Alert"] == 0:
      if left_lane_depart:
        values["LKAS_Alert"] = 12 # Left lane departure dash alert
      elif right_lane_depart:
        values["LKAS_Alert"] = 11 # Right lane departure dash alert

    values["LKAS_ACTIVE"] = 1 # Show LKAS lane lines
    values["LKAS_Dash_State"] = 1 # LKAS white standby indicator
    
    values["LKAS_Left_Line_Enable"] = 1 # Enable LKAS left lane line
    values["LKAS_Right_Line_Enable"] = 1 # Enable LKAS right line 
    
    values["LKAS_Left_Line_Visible"] = 1
    values["LKAS_Right_Line_Visible"] = 1

    if enabled:
      
      values["LKAS_Dash_State"] = 2 # Green enabled indicator
      
      values["LKAS_Left_Line_Visible"] = 2
      values["LKAS_Right_Line_Visible"] = 2      

  elif (lkas_mode == 2):

    if mchange:
      
      values["LKAS_ACTIVE"] = 1
      values["LKAS_Left_Line_Enable"] = 1 # Enable LKAS left lane line
      values["LKAS_Right_Line_Enable"] = 1 # Enable LKAS right line   
      
  elif (lkas_mode == 1):
    #if mchange:
    values["LKAS_Dash_State"] = 0 # LKAS OFF
    values["LKAS_Left_Line_Enable"] = 0 # Enable LKAS left lane line
    values["LKAS_Right_Line_Enable"] = 0 # Enable LKAS right line 
    values["LKAS_ACTIVE"] = 0
    
  
  if sng_acc_resume:
    values["LKAS_Left_Line_Enable"] = 0 # Show LKAS left lane line
    values["LKAS_Right_Line_Enable"] = 0 # Show LKAS right line

  return packer.make_can_msg("ES_LKAS_State", 0, values)

def create_es_dashstatus(lkas_mode, packer, dashstatus_msg):
  values = copy.copy(dashstatus_msg)

  # Filter stock LKAS disabled and Keep hands on steering wheel OFF alerts
  if values["LKAS_State_Msg"] in [2, 3]:
    values["LKAS_State_Msg"] = 0
    
  if (lkas_mode > 1):
    values["Signal3"] = 1        #LKAS_dash icon ON

  return packer.make_can_msg("ES_DashStatus", 0, values)

def create_cruise_buttons(packer, sw_cruise_buttons_msg, cspeed_dn_cmd, cspeed_up_cmd):
  values = copy.copy(sw_cruise_buttons_msg)

  # Find the correct variable
  values["Signal2"] = 0
  
  if ((cspeed_dn_cmd == True) and (cspeed_up_cmd == False)):
    values["Set"] = 1    #JP Test
    
  if ((cspeed_up_cmd == True) and (cspeed_dn_cmd == False)):
    values["Resume"] = 1
    
  return packer.make_can_msg("Cruise_Buttons", 0, values)

def create_throttle(packer, throttle_msg, throttle_cmd):

  values = copy.copy(throttle_msg)
  if throttle_cmd:
    values["Throttle_Pedal"] = 5

  return packer.make_can_msg("Throttle", 2, values)

def create_brake_pedal(packer, brake_pedal_msg, speed_cmd, brake_cmd):

  values = copy.copy(brake_pedal_msg)
  if speed_cmd:
    values["Speed"] = 3
  if brake_cmd:
    values["Brake_Pedal"] = 5
    values["Brake_Lights"] = 1

  return packer.make_can_msg("Brake_Pedal", 2, values)

# *** Subaru Pre-global ***

def subaru_preglobal_checksum(packer, values, addr):
  dat = packer.make_can_msg(addr, 0, values)[2]
  return (sum(dat[:7])) % 256

def create_preglobal_steering_control(packer, apply_steer):
  values = {
    "LKAS_Command": apply_steer,
    "LKAS_Active": 1 if apply_steer != 0 else 0
  }
  values["Checksum"] = subaru_preglobal_checksum(packer, values, "ES_LKAS")

  return packer.make_can_msg("ES_LKAS", 0, values)

def create_preglobal_es_distance(packer, cruise_button, es_distance_msg):

  values = copy.copy(es_distance_msg)
  values["Cruise_Button"] = cruise_button

  values["Checksum"] = subaru_preglobal_checksum(packer, values, "ES_Distance")

  return packer.make_can_msg("ES_Distance", 0, values)
