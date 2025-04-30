#include "gravity360_encoder.h"

namespace esphome {
namespace gravity360_encoder {

static const char *TAG = "gravity360_encoder";

void Gravity360Encoder::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Gravity 360\u00B0 Rotary Encoder (SEN0502)...");
  // Verify device by reading PID
  uint8_t pid_data[2] = {0, 0};
  bool ok = this->read_bytes(GRAV360_REG_PID_MSB, pid_data, 2);
  if (!ok) {
    ESP_LOGE(TAG, "Communication with device failed at I2C address 0x%02X", this->address_);
    this->mark_failed();
    return;
  }
  uint16_t pid = (uint16_t(pid_data[0]) << 8) | pid_data[1];
  if (pid != GRAV360_DEVICE_PID) {
    ESP_LOGE(TAG, "Unexpected device PID 0x%04X at address 0x%02X (expected 0x%04X)", pid, this->address_, GRAV360_DEVICE_PID);
    this->mark_failed();
    return;
  }
  // Register API services for setting value and gain
  this->register_service(&Gravity360Encoder::set_encoder_value, "set_encoder_value", {"value"});
  this->register_service(&Gravity360Encoder::set_gain_coefficient, "set_gain_coefficient", {"value"});
}

void Gravity360Encoder::update() {
  // Read current encoder count (16-bit value from two registers)
  uint8_t buf[2];
  if (this->read_bytes(GRAV360_REG_COUNT_MSB, buf, 2)) {
    uint16_t value = (uint16_t(buf[0]) << 8) | buf[1];
    this->publish_state(value);  // update sensor value
  } else {
    ESP_LOGW(TAG, "Failed to read encoder value");
  }
  // Check and handle button press events
  if (this->button_ != nullptr) {
    // If last cycle had a press, clear the binary sensor now
    if (this->button_active_) {
      this->button_->publish_state(false);
      this->button_active_ = false;
    }
    uint8_t key_status = 0;
    if (this->read_byte(GRAV360_REG_KEY_STATUS, &key_status)) {
      if (key_status & 0x01) {
        // Clear the pressed flag on the device
        uint8_t clear_val = 0x00;
        this->write_byte(GRAV360_REG_KEY_STATUS, clear_val);
        // Publish button pressed state
        this->button_->publish_state(true);
        this->button_active_ = true;  // mark to reset next cycle
      }
    } else {
      ESP_LOGW(TAG, "Failed to read button status");
    }
  }
}

void Gravity360Encoder::dump_config() {
  ESP_LOGCONFIG(TAG, "Gravity360Encoder:");
  ESP_LOGCONFIG(TAG, "  I2C Address: 0x%02X", this->address_);
  ESP_LOGCONFIG(TAG, "  Update Interval: %u ms", this->get_update_interval());
  if (this->is_failed()) {
    ESP_LOGCONFIG(TAG, "  Status: FAILED (device not found or wrong type)");
  }
}

void Gravity360Encoder::set_encoder_value(int value) {
  // Clamp value to valid range 0–1023 and write to device
  if (value < 0) value = 0;
  if (value > 1023) value = 1023;
  uint8_t data[2];
  data[0] = (uint8_t)((value >> 8) & 0xFF);
  data[1] = (uint8_t)(value & 0xFF);
  bool ok = this->write_bytes(GRAV360_REG_COUNT_MSB, data, 2);
  if (!ok) {
    ESP_LOGW(TAG, "Failed to set encoder value to %d", value);
  } else {
    this->publish_state(value);  // update sensor immediately
  }
}

void Gravity360Encoder::set_gain_coefficient(int gain) {
  // Clamp gain to valid range 1–51 and write to device
  if (gain < 1 || gain > 51) {
    ESP_LOGW(TAG, "Invalid gain %d (valid range: 1-51)", gain);
    return;
  }
  uint8_t gain_val = static_cast<uint8_t>(gain);
  if (!this->write_byte(GRAV360_REG_GAIN, gain_val)) {
    ESP_LOGW(TAG, "Failed to set gain coefficient to %d", gain);
  } else {
    ESP_LOGD(TAG, "Gain coefficient set to %d", gain);
  }
}

}  // namespace gravity360_encoder
}  // namespace esphome
