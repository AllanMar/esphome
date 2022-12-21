#pragma once

#include "Wire.h"
#include "esphome/core/log.h"
#include <string>

namespace esphome
{
    namespace boilermon
    {

#define BOILERI2C_PACKET_SIZE 6
#define BOILERI2C_MIN_MSGCOUNT 20
#define DISPLAY_DIGITS 4
#define INFOMENU_MAX 29
#define INFOMENU_ARRAYLEN 4

        const char paraArray[] = {'P', 'A', 'R', 'A'};     // PARM
        const char infoArray[] = {0x31, 0x4e, 0x46, 0x30}; // INFO
        const char stbyArray[] = {'5', 'T', 'B', 'Y'};     // STBY
        const char warnArray[] = {'B', 0, 0, 0};           //"b" warning code

        const char segLookup[][2] = {{63, '0'},
                                     {6, '1'},
                                     {91, '2'},
                                     {79, '3'},
                                     {102, '4'},
                                     {109, '5'},
                                     {125, '6'},
                                     {39, '7'},
                                     {127, '8'},
                                     {111, '9'},
                                     {119, 'A'},
                                     {124, 'B'},
                                     {57, 'C'},
                                     {94, 'D'},
                                     {121, 'E'},
                                     {113, 'F'},
                                     {61, 'G'},
                                     {118, 'H'},
                                     {48, 'I'},
                                     {115, 'P'},
                                     {49, 'R'},
                                     {55, 'N'},
                                     {110, 'Y'},
                                     {120, 'T'},
                                     {56, 'L'},
                                     {14, 'J'},
                                     {0, ' '},
                                     {64, '-'},
                                     {16, 'e'},
                                     {4, 'c'}};
#define SEG_LOOKUP_SIZE 30

        class BoilerI2C
        {
        public:
            
            enum MenuType
            {
                NONE,
                PARA_MENU ='P',
                INFO_MENU = 'I',
                STBY_MENU = 'S',
                PARA_DISP = 'p',
                STBY_DISP= 's',
                INFO_DISP= 'i',
                WARN_DISP = 'w',
                UNK= '?'
            };

            struct DisplayData
            {
                char digits[DISPLAY_DIGITS];
                char dec_count;

                //MenuType menu_type;              
                //uint8_t mode;
                //unsigned int value;

                uint8_t get_mode() {
                    uint8_t mode_digit = digits[0];
                    return mode_digit;
                }
                int16_t get_value() {
                    return atoi(&digits[1]);
                }
                MenuType get_menu() {
                    if (!memcmp(digits,&stbyArray, 4))
                        return STBY_MENU;
                    else if (!memcmp(digits,&paraArray, 4))
                        return PARA_MENU;
                    else if (!memcmp(digits,&infoArray, 4))
                        return INFO_MENU;
                    else if (dec_count==0)
                        return STBY_DISP;
                    else if (dec_count==1)
                        return PARA_DISP;
                    else if (dec_count>1)
                        return INFO_DISP;  
                    else if (!memcmp(digits,&warnArray, 1))
                        return WARN_DISP;
                    else 
                        return UNK;
                    }
                bool operator==(const DisplayData &other) const
                {
                    bool ret_val = this->digits[0] == other.digits[0] 
                    && this->digits[1] == other.digits[1] 
                    && this->digits[2] == other.digits[2] 
                    && this->digits[3] == other.digits[3] 
                    && this->dec_count == dec_count;
                    return ret_val;
                }
                bool operator!=(const DisplayData &other) const
                {
                    return !(*this == other);
                }
            };
            DisplayData display;
            static const __FlashStringHelper *get_msg_type_text(MenuType menu_type)
            {
                switch (menu_type)
                {
                case NONE:
                    return F("None");
                case PARA_MENU:
                    return F("PARA Menu");
                case INFO_MENU:
                    return F("INFO Menu");
                case STBY_MENU:
                    return F("STBY Menu");
                case PARA_DISP:
                    return F("PARM");
                case STBY_DISP:
                    return F("STBY");
                case INFO_DISP:
                    return F("INFO");
                case WARN_DISP:
                    return F("WARN");
                default:
                    return F("UNKNOWN");
                }
            }
            bool begin(uint8_t slave_address, uint8_t sda_pin, uint8_t scl_pin);
            void loop();
            
        bool available() { return available_; }
        bool connected() { return connected_; }

        DisplayData get_display();
        //void set_sda_pin (uint8_t pin) { sda_pin_ = pin; }
        //void set_scl_pin (uint8_t pin) { scl_pin_ = pin; }
        //void set_slave_address (uint8_t address) { slave_address_ = address; }

        private:
            bool connected_ = false;
            bool available_ = false;

            //uint8_t sda_pin_;
            //uint8_t scl_pin_;
            //uint8_t slave_address_;

            volatile static uint8_t DRAM_ATTR i2c_data_[BOILERI2C_PACKET_SIZE];
            volatile static unsigned long DRAM_ATTR i2c_last_time_;
            volatile static uint8_t DRAM_ATTR i2c_data_ready_;
            volatile static uint8_t DRAM_ATTR i2c_count_;
            volatile static uint8_t DRAM_ATTR dec_count_;
            static void IRAM_ATTR receive_isr(int howMany);
            char convert_seg_(uint8_t i)
            {
                for (uint8_t x = 0; x < SEG_LOOKUP_SIZE; x++)
                {
                    if (i == segLookup[x][0])
                        return char(segLookup[x][1]);
                }
                return i; // Return uncoverted charecter.
            }
        
        };
    }
}