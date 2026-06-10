# Installation

## Necessary Home Assistant Automations

The RFID cards have to be added as tags to home assistant. Simply scan them in home assistant and assign the music id (e.g. "library://artist/204", "library://album/140", "library://playlist/48") as the name of the tag. 

Because ESPHome devices cannot read the name of a tag directly, a Home Assistant automation is used to read the tag name and call the ESPHome service to play the media. To get the device id, go to developer tools and listen for the tag_scanned event. Then scan a tag on your device and it will show the device id.

```yaml
alias: Media Player Tablet tag scanned
description: ""
triggers:
  - trigger: event
    event_type: tag_scanned
    event_data:
      device_id: your_esphome_device_id
conditions: []
actions:
  - action: esphome.media_player_tablet_play_nfc_media
    metadata: {}
    data:
      media_id: "{{ trigger.event.data.name }}"
mode: restart
```

## Necessary Home Assistant Helpers

Template Sensor necessary because fallback to media_artist when media_album_artist is unavailable is not possible in ESPHome:
```
# In Home Assistant - configuration.yaml or template sensor
template:
  - sensor:
      - name: "Media Player Display Artist"
        state: >
          {{ state_attr('media_player.your_player', 'media_album_artist') 
             or state_attr('media_player.your_player', 'media_artist') 
             or '' }}
```
