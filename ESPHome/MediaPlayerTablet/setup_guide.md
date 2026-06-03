# Media Player Tablet Setup Guide

Welcome to the Media Player Tablet! This guide will walk you through setting up the hardware and software for your premium smart tablet.

## 1. Hardware Requirements & Assembly

You will need the following parts:
- **M5Stack Tab5** (The main tablet screen and processor)
- **M5Stack Unit Scroll** (The rotary encoder wheel for volume control)
- **M5Stack Unit RFID 2** (The card reader for music selection)
- **Two 10cm Grove Cables**
- **One Grove T-Connector**

### How to Connect:
1. Plug one Grove cable into the port on the **M5Stack Tab5**.
2. Connect the other end of this cable into the **Grove T-Connector**.
3. Plug the **M5Stack Unit RFID 2** directly into one side of the T-Connector.
4. Use the second Grove cable to connect the other side of the T-Connector to the **M5Stack Unit Scroll**.

*(This setup links both the scroll wheel and the RFID reader to the tablet via the same communication line.)*

## 2. Software Configuration

### ESPHome Setup
1. In your Home Assistant dashboard, open the **ESPHome** add-on.
2. Create a new device and upload the provided `mediaplayertablet.yaml` configuration.
3. You will need to define a few secret values in your ESPHome `secrets.yaml` file (such as your WiFi credentials, `ha_url` for your Home Assistant local URL, and `ha_token` for a long-lived access token).
4. Install the configuration onto the Tab5 via a USB-C cable for the first time.

### Home Assistant Integration
1. Once the tablet connects to your WiFi, Home Assistant will auto-discover it. Add it to your integrations.
2. **Important:** When adding the ESPHome device in Home Assistant, you must check the box that says **"Allow the device to perform Home Assistant actions"**. This gives the tablet permission to control your music and volume.

## 3. Music Assistant Configuration

The tablet gets its music from **Music Assistant** running on your Home Assistant server.
1. Make sure your physical smart speaker (e.g., Sonos, HomePod, or local speaker) is added to Home Assistant.
2. Open Music Assistant and add that Home Assistant Media Player as a player in Music Assistant.
3. This creates a special "Music Assistant" version of your speaker.
4. The entity ID of this Music Assistant speaker is the one you must use in your `mediaplayertablet.yaml` file (under the `media_player_entity` substitution).

## 4. Setting up RFID Cards

Because ESPHome cannot natively look up the names of your RFID tags from the HA database, we use a simple automation.

1. Go to Home Assistant -> Settings -> Tags.
2. Scan a new RFID card on the tablet. It will show up in Home Assistant.
3. Name the tag with the exact ID of the artist, album, or playlist from your Music Assistant library (e.g., `library://artist/204`, `library://album/140`, or `library://playlist/48`).
4. Create an automation in Home Assistant that listens for a tag scanned event from your tablet and calls the tablet's ESPHome service to play the media. 

Now, simply place a card on the reader and enjoy your music!
