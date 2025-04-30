#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace gravity360_encoder {

// I2C register addresses and expected device ID
static const uint8_t GRAV360_REG_PID_MSB    = 0x00;
static const uint8_t GRAV360_REG_PID_LSB    = 0x01;
static const uint8_t GRAV360_REG_COUNT_MSB  = 0x08;
static const uint8_t GRAV360_REG_COUNT_LSB  = 0x09;
static const uint8_t GRAV360_REG_KEY_STATUS = 0x0A;
static const uint8_t GRAV360_REG_GAIN       = 0x0B;
static const uint16_t GRAV360_DEVICE_PID    = 0x01F6;  // Expected PID for SEN0502

class Gravity360Encoder : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
  public:
    // Link the binary sensor for the button
    void set_button_sensor(binary_sensor::BinarySensor *button) { this->button_ = button; }

    void setup() override;
    void update() override;
    void dump_config() override;

    // Service call handlers (to be exposed via ESPHome API)
    void set_encoder_value(int value);
    void set_gain_coefficient(int gain);

  protected:
    binary_sensor::BinarySensor *button_{nullptr};
    bool button_active_{false};  // Tracks a button press event to reset state
};

}  // namespace gravity360_encoder
}  // namespace esphome
