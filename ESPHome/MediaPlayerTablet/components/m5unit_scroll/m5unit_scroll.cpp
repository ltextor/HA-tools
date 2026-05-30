#include "m5unit_scroll.h"
#include "esphome/core/log.h"

namespace esphome {
namespace m5unit_scroll {

static const char *const TAG = "m5unit_scroll";

void M5UnitScroll::setup() {
  ESP_LOGCONFIG(TAG, "Setting up M5Unit Scroll...");
  // Ensure both LEDs are off on every boot/reset, regardless of prior state.
  // Without this, the encoder module (which stays powered during software resets)
  // retains whatever LED colour it had before the reset.
  this->set_led_color(0, 0x000000);
  this->set_led_color(1, 0x000000);
}

void M5UnitScroll::dump_config() {
  LOG_SENSOR("", "M5Unit Scroll (Encoder)", this);
  LOG_I2C_DEVICE(this);
}

void M5UnitScroll::update() {
  const int16_t enc = this->read_encoder_value_();
  // Only publish when the absolute position has actually changed.
  // last_encoder_value_ is initialised to INT16_MIN so the very first read
  // is always treated as a change (captures the true initial position),
  // but subsequent idle polls at the same value are suppressed.
  if (enc != this->last_encoder_value_) {
    this->last_encoder_value_ = enc;
    this->publish_state(enc);
  }

  if (this->increment_sensor_ != nullptr) {
    const int16_t inc = this->read_increment_value_();
    // Only publish when there is actual movement — avoids triggering on_value
    // callbacks at every poll interval when the encoder is idle.
    if (inc != 0) {
      this->increment_sensor_->publish_state(inc);
    }
  }

  if (this->button_sensor_ != nullptr) {
    const bool pressed = this->read_button_pressed_();
    this->button_sensor_->publish_state(pressed);
  }

  // Flush any LED state changes that arrived since the last update().
  // The rgb light platform sets R, G, and B in three separate write_state()
  // calls, which would cause three I2C writes and flicker if we sent each
  // immediately. Deferring to here collapses them into one transaction.
  this->flush_leds_();
}

int16_t M5UnitScroll::read_encoder_value_() {
  uint8_t data[2];
  if (!this->read_bytes(ENCODER_REG, data, 2)) {
    ESP_LOGW(TAG, "Failed to read encoder value");
    return 0;
  }
  return (int16_t) ((data[0]) | (data[1] << 8));
}

int16_t M5UnitScroll::read_increment_value_() {
  uint8_t data[2];
  if (!this->read_bytes(INC_ENCODER_REG, data, 2)) {
    ESP_LOGW(TAG, "Failed to read increment value");
    return 0;
  }
  return (int16_t) ((data[0]) | (data[1] << 8));
}

bool M5UnitScroll::read_button_pressed_() {
  uint8_t data;
  if (!this->read_byte(BUTTON_REG, &data)) {
    ESP_LOGW(TAG, "Failed to read button status");
    return false;
  }
  // Official library: pressed when byte == 0x00
  return data == 0x00;
}

void M5UnitScroll::reset_encoder() {
  if (!this->write_byte(RESET_REG, 1)) {
    ESP_LOGW(TAG, "Failed to reset encoder");
  }
}

void M5UnitScroll::set_led_color(uint8_t index, uint32_t color) {
  // Official M5Unit Scroll library: setLEDColor(uint32_t color, uint8_t index)
  // writes [R, G, B] to register RGB_LED_REG + index * 3.
  //   LED 0 → 0x30, LED 1 → 0x33
  // The original code wrote [index, R, G, B] to a single register, which
  // put the index byte into the RED channel — causing both LEDs to map to the
  // same physical LED and leaving a dim red glow after "off" writes for LED 1.
  uint8_t data[3];
  data[0] = (color >> 16) & 0xFF;  // R
  data[1] = (color >> 8) & 0xFF;   // G
  data[2] = (color >> 0) & 0xFF;   // B

  uint8_t reg = RGB_LED_REG + index * 3;  // 0x30 for index=0, 0x33 for index=1
  if (!this->write_bytes(reg, data, 3)) {
    ESP_LOGW(TAG, "Failed to set LED %u color", index);
  }
}

void M5UnitScroll::set_led_channel(uint8_t index, uint8_t channel, uint8_t value) {
  if (index > 1 || channel > 2)
    return;
  if (led_state_[index][channel] == value)
    return;  // No change — skip dirty mark to avoid redundant flushes
  led_state_[index][channel] = value;
  led_dirty_[index] = true;
  // Do NOT write to I2C here. The rgb light calls write_state() three times
  // (once per channel) in rapid succession. Writing here would send three
  // separate I2C transactions with intermediate partial colours, causing
  // visible flicker. flush_leds_() in update() sends a single write after
  // all channels have been updated.
}

void M5UnitScroll::flush_leds_() {
  for (uint8_t i = 0; i < 2; i++) {
    if (led_dirty_[i]) {
      uint32_t color = ((uint32_t) led_state_[i][0] << 16) |  // R
                       ((uint32_t) led_state_[i][1] << 8) |   // G
                       ((uint32_t) led_state_[i][2]);          // B
      this->set_led_color(i, color);
      led_dirty_[i] = false;
    }
  }
}

void M5UnitScrollLEDOutput::write_state(float state) {
  if (parent_ == nullptr)
    return;
  // Clamp and convert 0.0–1.0 float to 0–255 byte
  if (state < 0.0f)
    state = 0.0f;
  if (state > 1.0f)
    state = 1.0f;
  parent_->set_led_channel(led_index_, channel_, (uint8_t)(state * 255.0f));
}

}  // namespace m5unit_scroll
}  // namespace esphome
