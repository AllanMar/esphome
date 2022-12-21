#include "boileri2c.h"
#include "esphome/core/helpers.h"

namespace esphome
{
namespace boilermon
{
    const char *TAG2 = "boilermon.boileri2c"; // Logging tag

    volatile uint8_t BoilerI2C::i2c_data_[BOILERI2C_PACKET_SIZE];
    volatile uint32_t BoilerI2C::i2c_last_time_ = 0;
    volatile uint8_t BoilerI2C::i2c_data_ready_ = false;
    volatile uint8_t BoilerI2C::i2c_count_ = 0;
    volatile uint8_t BoilerI2C::dec_count_ = 0;
    void IRAM_ATTR BoilerI2C::receive_isr_ (int recv_bytes)
    {
        uint8_t temp_data[BOILERI2C_PACKET_SIZE];
        uint8_t new_data_avail = false;

        if (recv_bytes== BOILERI2C_PACKET_SIZE)
        {
            i2c_last_time_ = millis();
            for (int x = 0; x < BOILERI2C_PACKET_SIZE; x++)
            {
                temp_data[x] = Wire.read();
                if ((temp_data[x] & 0b01111111) != (i2c_data_[x] & 0b01111111)) // Ignore decimal segment changes here
                    new_data_avail = true;
            }
            if (new_data_avail)
            {
                for (int x = 0; x < BOILERI2C_PACKET_SIZE; x++)
                { // Update last Data
                    i2c_data_[x] = temp_data[x];
                }
                i2c_count_ = 0;                                   // Reset Rcv Counter
                dec_count_ = ((temp_data[2] & 0b10000000) ? 1 : 0); // If first digit has decimal set increment dec_count_er (to deal with non-flashing case)
            }
            else
            {
                if ((temp_data[2] != i2c_data_[2]) && (dec_count_ < 255))
                { // Only decimal changed. Count to determine if it is flashing
                    dec_count_++;
                }
                if (i2c_count_ < 255)
                    i2c_count_++;
                if (i2c_count_ == BOILERI2C_MIN_MSGCOUNT)
                { // Recieved the minimum number of messages to action.
                    i2c_data_ready_ = true;
                }
            }
        }
        else
        {
            while (Wire.available()) // Read and discard any Packet beyond valid length.
            {
                uint8_t c = Wire.read(); // receive a uint8_t as character
            }
        }
    }
    bool BoilerI2C::begin(uint8_t slave_address, uint8_t sda_pin, uint8_t scl_pin)
    {
        // This will be called once to set up the component
        // think of it as the setup() call in Arduino
        connected_ = Wire.begin(slave_address, sda_pin, scl_pin, 0);
        if (connected_ )
           Wire.onReceive(receive_isr_);

        return connected_;
    }
    
    void BoilerI2C::loop()
    {
        read_();
    }
    void IRAM_ATTR BoilerI2C::read_() {
        InterruptLock lock;
        if (i2c_data_ready_)
        {
           for (uint8_t x = 2; x < BOILERI2C_PACKET_SIZE; x++)
                display.digits[x - 2] = convert_seg_(i2c_data_[x] & 0b01111111); // Remove DP
                display.dec_count = dec_count_;
                i2c_data_ready_ = false;
            available_ = true;
        }
    }
    BoilerI2C::DisplayData BoilerI2C::get_display()
    {
        available_ = false;
        return display;

    }        
}
}