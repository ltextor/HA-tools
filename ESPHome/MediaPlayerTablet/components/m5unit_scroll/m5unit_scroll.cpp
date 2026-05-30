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
  // Official library writes: [index, R, G, B] to RGB_LED_REG
  uint8_t data[4];
  data[0] = index;
  data[1] = (color >> 16) & 0xFF;  // R
  data[2] = (color >> 8) & 0xFF;   // G
  data[3] = (color >> 0) & 0xFF;   // B

  if (!this->write_bytes(RGB_LED_REG, data, 4)) {
    ESP_LOGW(TAG, "Failed to set LED color");
  }
}

void M5UnitScroll::set_led_channel(uint8_t index, uint8_t channel, uint8_t value) {
  if (index > 1 || channel > 2)
    return;
  led_state_[index][channel] = value;
  uint32_t color = ((uint32_t) led_state_[index][0] << 16) |  // R
                   ((uint32_t) led_state_[index][1] << 8) |   // G
                   ((uint32_t) led_state_[index][2]);          // B
  this->set_led_color(index, color);
}

void M5UnitScrollLEDOutput::write_state(float state) {
  if (parent_ == nullptr)
    return;
  // Clamp and convert 0.0–1.0 float to 0–255 byte
  if (state < 0.0f) state = 0.0f;
  if (state > 1.0f) state = 1.0f;
  parent_->set_led_channel(led_index_, channel_, (uint8_t)(state * 255.0f));
}

}  // namespace m5unit_scroll
}  // namespace esphome
