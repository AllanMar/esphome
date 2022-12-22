#pragma once
//#include "boileri2c.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/gpio.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/api/custom_api_device.h"

namespace esphome {
namespace boilermon {

#define MSG_TIMEOUT 1000
/*
static std::string flash_to_string(const __FlashStringHelper *flash) {
  std::string result;
  const char *fptr = (PGM_P) flash;
  result.reserve(strlen_P(fptr));
  char c;
  while ((c = pgm_read_byte(fptr++)) != 0)
    result.push_back(c);
  return result;
}
*/
//char infoMenuArray[INFOMENU_MAX][INFOMENU_ARRAYLEN];

//int tempArray[7] = {0,0,0,0,0,0,0};
//int rateArray[3] = {0,0,0};
//unsigned long counterArray[4] = {0,0,0,0};
//uint8_t statusMode=0;
//uint8_t statusVal=0;
//bool newStatus=false;

class BoilerMonComponent : public PollingComponent, public api::CustomAPIDevice {
public:
  sensor::Sensor *value_sensor_ {nullptr};

  sensor::Sensor *supply_temp_sensor_ {nullptr};
  sensor::Sensor *return_temp_sensor_ {nullptr};
  sensor::Sensor *dhw_temp_sensor_ {nullptr};
  sensor::Sensor *outdoor_temp_sensor_ {nullptr};
  sensor::Sensor *flue_temp_sensor_ {nullptr};
  sensor::Sensor *setpoint_temp_sensor_ {nullptr};

  text_sensor::TextSensor *status_text_sensor_ {nullptr};
  InternalGPIOPin *sda_pin_ {nullptr};
  InternalGPIOPin *scl_pin_ {nullptr};
  InternalGPIOPin *mode_pin_ {nullptr};
  InternalGPIOPin *step_pin_ {nullptr};
  uint8_t address_;
  //ESPHome API Calls
  void set_value_sensor(sensor::Sensor *value_sensor) { value_sensor_ = value_sensor; }
  void set_status_text_sensor(text_sensor::TextSensor *status_text_sensor) { status_text_sensor_ = status_text_sensor; }

  void set_supply_temp_sensor(sensor::Sensor *supply_temp_sensor) { supply_temp_sensor_ = supply_temp_sensor; }
  void set_return_temp_sensor(sensor::Sensor *return_temp_sensor) { return_temp_sensor_ = return_temp_sensor; }
  void set_dhw_temp_sensor(sensor::Sensor *dhw_temp_sensor) { dhw_temp_sensor_ = dhw_temp_sensor; }
  void set_outdoor_temp_sensor(sensor::Sensor *outdoor_temp_sensor) { outdoor_temp_sensor_ = outdoor_temp_sensor; }
  void set_flue_temp_sensor(sensor::Sensor *flue_temp_sensor) { flue_temp_sensor_ = flue_temp_sensor; }
  void set_setpoint_temp_sensor(sensor::Sensor *setpoint_temp_sensor) { setpoint_temp_sensor_ = setpoint_temp_sensor; }

  void on_scan_menu(std::string menu, int length);

  void set_sda_pin(InternalGPIOPin *pin) { sda_pin_ = pin; }
  void set_scl_pin(InternalGPIOPin *pin) { scl_pin_ = pin; }
  void set_mode_pin(InternalGPIOPin *pin) { mode_pin_ = pin; }
  void set_step_pin(InternalGPIOPin *pin) { step_pin_ = pin; }
  void set_i2c_address(uint8_t address) { address_ = address; }

  float get_setup_priority() const { return setup_priority::LATE; }
  void dump_config();
  
  BoilerI2C::MenuType desired_menu = BoilerI2C::STBY_DISP;
  uint8_t desired_mode = '2';
  BoilerI2C MyBoiler;
  int tempArray[7] = {0,0,0,0,0,0,0};
  int rateArray[3] = {0,0,0};
  unsigned long counterArray[4] = {0,0,0,0};
  uint8_t statusMode=0;
  uint8_t statusVal=0;
  bool newStatus=false;
  BoilerMonComponent() : PollingComponent(5000) { }
  unsigned long lastGetInfo = 0;
  unsigned long lastStatusTime = 1;
  unsigned long currentTime = 0;
  void setup() override;
  static const __FlashStringHelper *get_status_text(uint8_t status);
  static std::string flash_to_string(const __FlashStringHelper *flash);
            
  void update() override;
  void loop() override;
     
};
}
}