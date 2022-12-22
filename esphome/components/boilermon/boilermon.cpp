#include "boileri2c.h"
#include "boilermon.h"

namespace esphome
{
namespace boilermon
{
    const uint16_t button_duration_ = 100;
    uint32_t last_mode_pin_on_ = 0;
    uint32_t last_step_pin_on_ = 0;
    uint8_t menu_count = 0;
    uint8_t menu_length = 0;
    bool menu_active=false;
    const char *TAG = "boilermon"; // Logging tag

    void BoilerMonComponent::dump_config() {
        ESP_LOGCONFIG(TAG, "BoilerMon:");
        ESP_LOGCONFIG(TAG, "  SDA Pin: %s", sda_pin_->dump_summary().c_str());
        ESP_LOGCONFIG(TAG, "  SCL Pin: %s", scl_pin_->dump_summary().c_str());
        ESP_LOGCONFIG(TAG, "  Mode Pin: %s", mode_pin_->dump_summary().c_str());
        ESP_LOGCONFIG(TAG, "  Step Pin: %s", step_pin_->dump_summary().c_str());
        ESP_LOGCONFIG(TAG, "  Address: 0x%x", address_);
        ESP_LOGCONFIG(TAG, "  Connected: %s", MyBoiler.connected() ? "Yes": "No");   
    }

    void BoilerMonComponent::setup() {
        // This will be called once to set up the component
        // think of it as the setup() call in Arduino
        mode_pin_->setup();
        step_pin_->setup();

        mode_pin_->pin_mode(gpio::FLAG_OUTPUT);
        step_pin_->pin_mode(gpio::FLAG_OUTPUT);

         register_service(&BoilerMonComponent::on_scan_menu, "scan_menu",
                     {"menu", "length"});

        if (!MyBoiler.begin(address_, sda_pin_->get_pin(), scl_pin_->get_pin()))
            this->mark_failed();

    }
    void BoilerMonComponent::on_scan_menu(std::string menu, int length) {
        desired_menu = (BoilerI2C::MenuType) menu.c_str()[0];
        //desired_mode = (uint8_t) mode.c_str()[0];
        menu_active = true;
        menu_length = (uint8_t)length;
        ESP_LOGD(TAG,"on_scan_menu(%c, %d)", desired_menu, menu_length);
        mode_pin_->digital_write(true); // Go to next Menu
        last_mode_pin_on_ = millis();
    }
    void BoilerMonComponent::update() {
        
    }

    void BoilerMonComponent::loop() {
        uint32_t now = millis();

        //turn off button if on longer than duration.
        if ((last_step_pin_on_>0)&&((now - last_step_pin_on_)> button_duration_)) {
            step_pin_->digital_write(false); 
            last_step_pin_on_ = 0;
        }
        if ((last_mode_pin_on_>0)&&((now - last_mode_pin_on_)> button_duration_)) {
            mode_pin_->digital_write(false); 
            last_mode_pin_on_ = 0;
        }
        MyBoiler.loop();

        if (MyBoiler.available()) {
            BoilerI2C::DisplayData new_message = MyBoiler.get_display();
            uint8_t item = new_message.get_mode();
            int16_t value = new_message.get_value();
            ESP_LOGD(TAG,"DISP: %c %c %i (%c%c%c%c)", new_message.get_menu(), new_message.get_mode(), new_message.get_value(), new_message.digits[0], new_message.digits[1],new_message.digits[2], new_message.digits[3]);
            
            if (new_message.get_menu() == BoilerI2C::STBY_DISP) {
                if (status_text_sensor_ != nullptr)
                    status_text_sensor_->publish_state(flash_to_string(get_status_text(new_message.get_mode())));
                if (value_sensor_ != nullptr)
                    value_sensor_->publish_state(new_message.get_value());
            } else if (new_message.get_menu() == BoilerI2C::INFO_DISP) {
                sensor::Sensor *sensor = nullptr;
                switch (item){
                    case '1':
                        sensor = supply_temp_sensor_;
                        break;
                    case '2':
                        sensor = return_temp_sensor_;
                        break;
                    case '3':
                        sensor = dhw_temp_sensor_;
                        break;
                    case '4':
                        sensor = outdoor_temp_sensor_;
                        break;
                    case '5':
                        sensor = flue_temp_sensor_;
                        break;
                    case '6':
                        sensor = setpoint_temp_sensor_;
                        break;
                }
                if (sensor != nullptr)
                    sensor->publish_state(value);
            }
            if (menu_active) {
                if (new_message.get_menu() == toupper(desired_menu)) { 
                    //ESP_LOGD(TAG, "Title Menu");
                    step_pin_->digital_write(true); // Go to next Menu
                    last_step_pin_on_ = millis();
                } else if (new_message.get_menu() == desired_menu) {
                    //ESP_LOGD(TAG, "Desired Menu (display)");
                    menu_count++;
                    if (menu_count>=menu_length||tolower(new_message.get_menu())==BoilerI2C::STBY_DISP) {
                        ESP_LOGD(TAG, "Scan complete");
                        menu_active=false; //We're Here.
                        menu_count=0;
                        if (tolower(new_message.get_menu())!=BoilerI2C::STBY_DISP) {
                            desired_menu = BoilerI2C::STBY_DISP;
                            menu_active = true;
                            mode_pin_->digital_write(true); // Go to next Menu
                            last_mode_pin_on_ = millis();
                        }
                    } else {
                        //ESP_LOGD(TAG, "Wrong Mode");
                        step_pin_->digital_write(true); // Go to next Menu
                        last_step_pin_on_ = millis();
                    }
                } else {
                    //ESP_LOGD(TAG, "Wrong Menu");
                    menu_count = 0;
                    mode_pin_->digital_write(true); // Go to next Menu
                    last_mode_pin_on_ = millis();
                }     
            }
        } 

  }
  const __FlashStringHelper *BoilerMonComponent::get_status_text(uint8_t status)
            {
                switch (status)
                {
                case '0':
                    return F("Standby");
                case '1':
                    return F("Fan pre/post purge");
                case '2':
                    return F("Ignition Sequence");
                case '3':
                    return F("Burner on CH");
                case '4':
                    return F("Burner on DHW");
                case '5':
                    return F("Pre-check for air flow");
                case '6':
                    return F("Burner off due to setpoint");
                case '7':
                    return F("CH post pump cycle");
                case '8':
                    return F("DHW post pump cycle");
                case '9':
                    return F("Burner blocked");
                default:
                    return F("UNKNOWN");
                }
            }
            std::string BoilerMonComponent::flash_to_string(const __FlashStringHelper *flash)
            {
                std::string result;
                const char *fptr = (PGM_P)flash;
                result.reserve(strlen_P(fptr));
                char c;
                while ((c = pgm_read_byte(fptr++)) != 0)
                    result.push_back(c);
                return result;
            }

}
}