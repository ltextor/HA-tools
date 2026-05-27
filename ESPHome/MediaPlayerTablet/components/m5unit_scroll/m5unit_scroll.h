#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace m5unit_scroll {

static const uint8_t SCROLL_ADDR = 0x40;

static const uint8_t ENCODER_REG = 0x10;
static const uint8_t BUTTON_REG = 0x20;
static const uint8_t RGB_LED_REG = 0x30;
static const uint8_t RESET_REG = 0x40;
static const uint8_t INC_ENCODER_REG = 0x50;

// Present in the official library (optional; not exposed in YAML here):
static const uint8_t BOOTLOADER_VERSION_REG = 0xFC;
static const uint8_t JUMP_TO_BOOTLOADER_REG = 0xFD;
static const uint8_t FIRMWARE_VERSION_REG = 0xFE;
static const uint8_t I2C_ADDRESS_REG = 0xFF;

class M5UnitScroll : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;

  void set_button_sensor(binary_sensor::BinarySensor *sensor) { this->button_sensor_ = sensor; }
  void set_increment_sensor(sensor::Sensor *sensor) { this->increment_sensor_ = sensor; }

  void reset_encoder();
  void set_led_color(uint8_t index, uint32_t color);

 protected:
  int16_t read_encoder_value_();
  int16_t read_increment_value_();
  bool read_button_pressed_();

  binary_sensor::BinarySensor *button_sensor_{nullptr};
  sensor::Sensor *increment_sensor_{nullptr};
};

}  // namespace m5unit_scroll
}  // namespace esphome
