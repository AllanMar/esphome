#include "boileri2c.h"
#include "boilermon.h"

namespace esphome
{
namespace boilermon
{
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

        if (!MyBoiler.begin(address_, sda_pin_->get_pin(), scl_pin_->get_pin()))
            this->mark_failed();

    }
    void BoilerMonComponent::update() {
        
    }
    void BoilerMonComponent::loop() {
        
        MyBoiler.loop();

        if (MyBoiler.available()) {
            BoilerI2C::DisplayData new_message = MyBoiler.get_display();
            ESP_LOGD(TAG,"DISP: %c %cs %i (%c%c%c%c)", new_message.get_menu(), new_message.get_mode(), new_message.get_value(), new_message.digits[0], new_message.digits[1],new_message.digits[2], new_message.digits[3]);
            
            if (new_message.get_menu() == BoilerI2C::STBY_DISP) {
                if (status_text_sensor_ != nullptr)
                status_text_sensor_->publish_state(flash_to_string(get_status_text(new_message.get_mode())));
                if (value_sensor_ != nullptr)
                value_sensor_->publish_state(new_message.get_value());
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