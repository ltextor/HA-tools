#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/output/float_output.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace m5unit_scroll {

static const uint8_t SCROLL_ADDR = 0x40;

static const uint8_t ENCODER_REG = 0x10;
static const uint8_t BUTTON_REG = 0x20;
static const uint8_t RGB_LED_REG = 0x30;  // 4-byte write: [index, R, G, B]
static const uint8_t RESET_REG = 0x40;
static const uint8_t INC_ENCODER_REG = 0x50;

// Present in the official library (optional; not exposed in YAML here):
static const uint8_t BOOTLOADER_VERSION_REG = 0xFC;
static const uint8_t JUMP_TO_BOOTLOADER_REG = 0xFD;
static const uint8_t FIRMWARE_VERSION_REG = 0xFE;
static const uint8_t I2C_ADDRESS_REG = 0xFF;

// Forward declaration required by M5UnitScrollLEDOutput
class M5UnitScroll;

/// Represents one colour channel (R, G or B) of one of the scroll encoder LEDs.
/// Created by the `output:` platform; combine three channels per LED
/// into a `light: platform: rgb` entity in YAML.
///
/// Protocol: official M5Unit Scroll library writes [index, R, G, B] (4 bytes)
/// to register RGB_LED_REG (0x30). Both physical LEDs share this register;
/// data[0] (index) selects which LED (0 or 1).
class M5UnitScrollLEDOutput : public output::FloatOutput {
 public:
  void set_parent(M5UnitScroll *parent) { parent_ = parent; }
  void set_led_index(uint8_t index) { led_index_ = index; }
  /// channel: 0 = R, 1 = G, 2 = B
  void set_channel(uint8_t channel) { channel_ = channel; }

  void write_state(float state) override;

 protected:
  M5UnitScroll *parent_{nullptr};
  uint8_t led_index_{0};
  uint8_t channel_{0};
};

class M5UnitScroll : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;

  void set_button_sensor(binary_sensor::BinarySensor *sensor) { this->button_sensor_ = sensor; }
  void set_increment_sensor(sensor::Sensor *sensor) { this->increment_sensor_ = sensor; }

  void reset_encoder();

  /// Write a full 24-bit colour (0x00RRGGBB) to one LED.
  /// Sends [index, R, G, B] to register RGB_LED_REG (0x30) — the official
  /// M5Unit Scroll protocol. Both LEDs share the same register; index selects which.
  void set_led_color(uint8_t index, uint32_t color);

  /// Update one channel of an LED and mark it dirty for the next flush.
  /// The actual I2C write is deferred to flush_leds_() inside update(),
  /// so all three R/G/B channel writes (which the rgb light makes in quick
  /// succession) are collapsed into a single I2C transaction — eliminating flicker.
  void set_led_channel(uint8_t index, uint8_t channel, uint8_t value);

 protected:
  int16_t read_encoder_value_();
  int16_t read_increment_value_();
  bool read_button_pressed_();

  /// Push any dirty LED state to hardware. Called at the end of update().
  void flush_leds_();

  binary_sensor::BinarySensor *button_sensor_{nullptr};
  sensor::Sensor *increment_sensor_{nullptr};
  int16_t last_encoder_value_{INT16_MIN};  // sentinel: forces one publish on first poll

  /// Cached per-channel brightness so partial writes can recompose full RGB.
  /// led_state_[led_index][channel] where channel: 0=R, 1=G, 2=B.
  uint8_t led_state_[2][3]{};

  /// True when the corresponding LED's state has changed since the last flush.
  bool led_dirty_[2]{};
};

}  // namespace m5unit_scroll
}  // namespace esphome
