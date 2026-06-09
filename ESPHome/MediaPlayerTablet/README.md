# Installation

## Necessary Home Assistant Automations

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
