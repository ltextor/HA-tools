# Media Player Tablet: Hardware & Software Setup Guide

This guide details the hardware modifications and software configuration required to set up the ESPHome-powered Media Player Tablet running on the **M5Stack Tab5** (specifically the V2 revision with the ST7123 display driver).

---

## Hardware Architecture & Modifications

The system utilizes the **M5Stack Tab5** (a dual-SoC device with an ESP32-P4 and ESP32-C6) as the core HMI. To achieve full media player capability (rotary volume control, RFID music selection, and high-quality local playback), several hardware components are integrated:

### 1. Core Device: M5Stack Tab5 (V2)
- **Processor:** ESP32-P4 (Main application/LVGL engine) & ESP32-C6 (Hosted Wi-Fi/Bluetooth bridge).
- **Display:** 5-inch 1280x720 MIPI DSI screen utilizing the integrated **ST7123** display-touch driver.
- **Audio:** ES8388 DAC routing to the internal speaker or 3.5mm headphone jack.
- **Power:** Onboard battery (NP-F550 type) with charging management and INA226 voltage/current telemetry.

### 2. External I2C Peripheral Modifications
To enable user interaction, the following M5Stack Unit modules are connected to the Tab5's external I2C bus (`GPIO53` / `GPIO54`):

- **M5Unit Encoder (Rotary Encoder):**
  - Communicates via I2C at address `0x40`.
  - Allows tactile volume control (scrolling) and media play/pause toggle (clicking the encoder button).
- **M5Unit RFID 2 (RFID Reader):**
  - Communicates via I2C at address `0x28`.
  - Scans physical cards to trigger Home Assistant automations for loading playlists, artists, or albums.

---

## Software Configuration

The software setup consists of two parts: the ESPHome firmware and the Home Assistant integration.

### 1. ESPHome Firmware Configuration
The firmware is built using the ESP-IDF framework to support the advanced features of the ESP32-P4. Key highlights of the configuration:

- **Wi-Fi Bridge (`esp32_hosted`):** Establishes the communication layer between the ESP32-P4 and the ESP32-C6 Wi-Fi co-processor.
- **Power Management (`esp_ldo`):** Supplies 1.1V to the MIPI DSI logic (channel 4) and 2.7V to the analog power rail (channel 3) to correctly power the ST7123 display.
- **LVGL Integration:** Renders a modern, responsive HMI dashboard displaying:
  - System Test Screen title.
  - "Currently Playing" track title and artist (synced from Home Assistant).
  - Battery percentage and charging status (via INA226 and battery template).
- **External Components:** Integrates custom drivers (e.g. `m5unit_scroll` / `m5unit_encoder`) sourced from git.

### 2. Home Assistant Integration

For full functionality, the ESPHome device must seamlessly communicate with your Home Assistant instance.

#### Service Call Authorization
Because the tablet makes service calls to Home Assistant (e.g. `media_player.media_play_pause` and `homeassistant.tag_scanned`), **the device must be allowed to make API calls/service calls**.
- During the integration setup of ESPHome in Home Assistant, ensure you authorize the device to execute service calls.
- In the ESPHome integration dashboard in Home Assistant, locate the **Media Player Tablet** card, click the three dots, select **Configure**, and verify that the checkbox **"Allow the device to make Home Assistant service calls"** is enabled.

#### Media Source & Queue
- **Music Assistant:** Install the Music Assistant integration in Home Assistant. It streams Spotify and local files to the media player entities.
- **Queue Synchronization:** The system listens to the state of `media_player.local_media_player` to fetch attributes like `media_title` and `media_artist` to update the tablet screen.
