# General
The project is a ESPHome tablet to play media either on the local speaker or on a (hard coded) home assistant media player. It runs on a M5Stack Tab5. It uses a M5Unit Unit Scroll encoder on I2C to control the volume and the play/pause of the media. It also has a built in battery and can be charged using USB-C. The YAML for the hardware is already implemented in mediaplayertablet.yaml.

Music is initially selected by scanning a RFID card, which selects an artist, album or playlist. From there, albums and tracks can be selected in the UI. The music is streamed from a local music assistant server (which in my case uses Spotify as music source, but it can access all music in the music assistant library).

The device should also implement the option to set a daily listening time in home assistant (ESPHome entity exposed to home assistant, reset daily by the device itself). Once the daily listening time has passed, the media player should not play any more music and show a "sleeping" screen saver. Before going into sleep mode, the current track should be finished. A button entity exposed to home assistant allows to reset the daily counter. If the daily listening time is set to 0, no limit is enforced.

# Hardware

## M5Stack Tab5
The tablet has a ESP32-P4 processor, a 5 inch touchscreen with 1280x720 px (should be used in landscape mode) and a built in battery. It has a USB-C port for charging and programming.

## M5Stack Unit Scroll
The M5Stack Unit Scroll is a rotary encoder with a built in button.
It uses the I2C interface to communicate with the tablet.
It has a RGB LED for status indication which will not be used in this project.

## M5Stack Unit RFID 2
The M5Stack Unit RFID 2 is a RFID reader.
It uses the I2C interface to communicate with the tablet.

# RFID cards
The RFID cards can contain the following keys (text items on position 0 and 1). The first entry specifies if the ID is an artist, album or playlist, the second record specifies the music assistant ID to that artist/album/playlist.

## Artist card
0   artist

1   *artist ID from Music Assistant* (e.g. "library://artist/204")

## Album card
0   album

1   *album ID from Music Assistant* (e.g. "library://album/140")

## Playlist card
0   playlist

1   *playlist ID from Music Assistant* (e.g. "library://playlist/48")

# UI
The UI is implemented using LVGL on ESPHome. It should have a very modern, minimalistic design (similar to Apple or Android design, including glassmorphism) and should be easy to use. The UI has to be responsive and should use smooth animations. Buttons should be big enough to make it easy for children to use. It is devided in several screens:

## Artist screen
The artist screen is shown when a RFID card with an artist ID is scanned.
It shows tiles (with album art and title) of the albums of the artist. When a album (tile) is pressed, the album screen is opened and the album starts playing immediately.

## Album or playlist screen
The album or playlist screen is shown when a album or playlist is opened from the artist screen or if a RFID card with an album or playlist ID is scanned.
It shows the album cover and some album information on the left side and a list of the tracks on the right side. When a track is pressed, the track is played. The list also highlights (e.g. by adding an animated icon) the track which is currently played.
There is a back button to go back to the artist screen if the album or playlist was opened from the artist screen. If the album or playlist was opened by scanning a RFID card, there is no back button.

## Media player area
The media player area is used to control the media being played. The media player area is always visible at the bottom of the screen. It can be expanded into a full screen media player view with all the buttons and also the cover art.
It has the following buttons/features: play/pause button, a next button, a previous button, a button to toggle repeat/repeat song/no repeat, a button to toggle shuffle, a track progress bar and a button to switch between local speaker and home assistant media player (use the transfer_queue service of the music assistant media player entities to move the queue to the other player).
Volume is controlled by the encoder on the I2C interface (when setting the volume it shows a slider on the screen as an overlay for a few seconds). Clicking the encoder plays or pauses the media.
When the RFID card is removed from the reader, the music is paused but the artist/album/playlist stays loaded until the next card is read or the device is reset. The user can then still restart the music or select a different track or album.

## RFID reading overlay
When the RFID reader detects a card, it should show an overlay on the screen with the artist/album/playlist name and information and the cover art. This overlay should be visible for a few seconds and then fade out.

## Battery and Wifi
Always show battery status (percentage and charging status) and wifi status on the top right corner of the screen.

## Daily listening time
If daily listening time is set, show the remaining listening time on the top left corner of the screen. When it has reached 0:00, show a sleeping screen saver and do not play any more music. The current track should finish playing before going into sleep mode. If no limit is set (value 0), this information should not be shown. A button entity exposed to home assistant allows to reset the daily counter.
The internal time (RTC module) should be synced with home assistant during boot. From then on it should work independently of home assistant (until the next reboot).

# Music information
All the music information comes from Music Assistant running on a local Home Assistant server. The following actions can be used to extract albums and tracks using the media player entity of the local device. The get_queue service returns information about the queue of the media player. The media player entity itself has attributes for album cover art, artist, track title, media position and duration, shuffle, repeat etc.

# Screen off
USB-power mode: Turn screen off when media player is not playing after 2 min idle (i.e. 2 min not playing without user interaction). Do not turn off screen when media player is playing.
Battery-power mode: Turn screen off after 30 seconds idle (i.e. 30 seconds not playing without user interaction). Turn off screen when media player is playing after 1 min without user interaction.

# Fonts
Use gfont Roboto for text. Use gfont Material Symbols & Icons (Material+Symbols+Outlined) for icons.

## Action to get albums of an artist
``
action: media_player.browse_media
target:
  entity_id: media_player.local_media_player
data:
  media_content_type: music
  media_content_id: library://artist/244
``

## Response
``
media_player.local_media_player:
  title: Kasperli
  media_class: artist
  media_content_type: artist
  media_content_id: library://artist/244
  children_media_class: album
  can_play: true
  can_expand: true
  can_search: false
  thumbnail: null
  not_shown: 0
  children:
    - title: Kasperli - De chalti Vulkan / De Förschter Sager und de Holzwurm Drill
      media_class: album
      media_content_type: music
      media_content_id: library://album/140
      children_media_class: null
      can_play: true
      can_expand: true
      can_search: false
      thumbnail: https://i.scdn.co/image/ab67616d0000b2731e2b495e7e1fd3d3fed3b178
    - title: Kasperli - De Räuber Pfäfferschnauz und de Chämifäger
      media_class: album
      media_content_type: music
      media_content_id: library://album/141
      children_media_class: null
      can_play: true
      can_expand: true
      can_search: false
      thumbnail: https://i.scdn.co/image/ab67616d0000b273389946d4069a903c5ed70d23
    - title: Kasperli - De Pinguin im Fischlisee / s'verschwundene Trottinett
      media_class: album
      media_content_type: music
      media_content_id: library://album/142
      children_media_class: null
      can_play: true
      can_expand: true
      can_search: false
      thumbnail: https://i.scdn.co/image/ab67616d0000b273e2dde61274be185d8f3ca167
    - ...
``

## Action to get tracks of album
``
action: media_player.browse_media
target:
  entity_id: media_player.local_media_player
data:
  media_content_type: music
  media_content_id: library://album/140
``

## Response
``
media_player.local_media_player:
  title: De chalti Vulkan / De Förschter Sager und de Holzwurm Drill
  media_class: album
  media_content_type: album
  media_content_id: library://album/140
  children_media_class: track
  can_play: true
  can_expand: true
  can_search: false
  thumbnail: null
  not_shown: 0
  children:
    - title: Kasperli - De chalti Vulkan - Teil 1
      media_class: track
      media_content_type: music
      media_content_id: spotify--g8pFWq7V://track/3bRDDXPg6CO8mFMTIUWs8n
      children_media_class: null
      can_play: true
      can_expand: false
      can_search: false
      thumbnail: https://i.scdn.co/image/ab67616d0000b2731e2b495e7e1fd3d3fed3b178
    - title: Kasperli - De chalti Vulkan - Teil 2
      media_class: track
      media_content_type: music
      media_content_id: spotify--g8pFWq7V://track/5gfDS0MKqjAbi3MeSkNzzn
      children_media_class: null
      can_play: true
      can_expand: false
      can_search: false
      thumbnail: https://i.scdn.co/image/ab67616d0000b2731e2b495e7e1fd3d3fed3b178
    - title: Kasperli - De chalti Vulkan - Teil 3
      media_class: track
      media_content_type: music
      media_content_id: spotify--g8pFWq7V://track/0EJCqE6CqQQLCH2si1xuEG
      children_media_class: null
      can_play: true
      can_expand: false
      can_search: false
      thumbnail: https://i.scdn.co/image/ab67616d0000b2731e2b495e7e1fd3d3fed3b178
    - ...
``

## Action to get tracks of playlist
``
action: media_player.browse_media
target:
  entity_id: media_player.local_media_player
data:
  media_content_type: music
  media_content_id: library://playlist/2
``

## Response
``
media_player.local_media_player:
  title: Random Artist (from library)
  media_class: playlist
  media_content_type: playlist
  media_content_id: library://playlist/2
  children_media_class: track
  can_play: true
  can_expand: true
  can_search: false
  thumbnail: null
  not_shown: 0
  children:
    - title: Robbie Williams - Let Love Be Your Energy
      media_class: track
      media_content_type: music
      media_content_id: library://track/537
      children_media_class: null
      can_play: true
      can_expand: false
      can_search: false
      thumbnail: https://i.scdn.co/image/ab67616d0000b273c5f3aaf3b54a777d96ecf604
    - title: Robbie Williams - Better Man
      media_class: track
      media_content_type: music
      media_content_id: library://track/538
      children_media_class: null
      can_play: true
      can_expand: false
      can_search: false
      thumbnail: https://i.scdn.co/image/ab67616d0000b273c5f3aaf3b54a777d96ecf604
    - title: Robbie Williams - Rock DJ
      media_class: track
      media_content_type: music
      media_content_id: library://track/539
      children_media_class: null
      can_play: true
      can_expand: false
      can_search: false
      thumbnail: https://i.scdn.co/image/ab67616d0000b273c5f3aaf3b54a777d96ecf604
    - ...
``

## Action to get queue
``
action: music_assistant.get_queue
target:
  entity_id: media_player.local_media_player
data: {}
``

## Response
``
media_player.local_media_player:
  queue_id: RINCON_B8E9375DF66801400
  active: true
  name: Badezimmer
  items: 9
  shuffle_enabled: false
  repeat_mode: "off"
  current_index: 0
  elapsed_time: 9
  current_item:
    queue_item_id: 5ddf0ad6f28c46e68c3f2f587011837d
    name: Ólafur Arnalds/Alice Sara Ott - Verses
    duration: 243
    media_item:
      media_type: track
      uri: library://track/455
      name: Verses
      version: ""
      image: https://i.scdn.co/image/ab67616d0000b273e64735fbacebe9f1a1519261
      favorite: true
      explicit: false
      artists:
        - media_type: artist
          uri: library://artist/39
          name: Ólafur Arnalds
          version: ""
          image: null
        - media_type: artist
          uri: library://artist/40
          name: Alice Sara Ott
          version: ""
          image: null
      album:
        media_type: album
        uri: library://album/41
        name: The Chopin Project
        version: ""
        image: https://i.scdn.co/image/ab67616d0000b273e64735fbacebe9f1a1519261
        favorite: true
        explicit: null
        discart_image: null
        artists:
          - media_type: artist
            uri: library://artist/39
            name: Ólafur Arnalds
            version: ""
            image: null
          - media_type: artist
            uri: library://artist/40
            name: Alice Sara Ott
            version: ""
            image: null
    stream_title: null
    stream_details:
      provider: spotify--g8pFWq7V
      item_id: 0uMgtxvlfFqa4T0YIMEDIj
      content_type: ogg
      sample_rate: 44100
      bit_depth: 16
      bit_rate: 320
  next_item:
    queue_item_id: 1ecca20c23424a2e90f28242b02f8cf0
    name: "Ólafur Arnalds/Alice Sara Ott/Frédéric Chopin - Piano Sonata No. 3: Largo"
    duration: 557
    media_item:
      media_type: track
      uri: library://track/456
      name: "Piano Sonata No. 3: Largo"
      version: ""
      image: https://i.scdn.co/image/ab67616d0000b273e64735fbacebe9f1a1519261
      favorite: true
      explicit: false
      artists:
        - media_type: artist
          uri: library://artist/39
          name: Ólafur Arnalds
          version: ""
          image: null
        - media_type: artist
          uri: library://artist/40
          name: Alice Sara Ott
          version: ""
          image: null
        - media_type: artist
          uri: library://artist/195
          name: Frédéric Chopin
          version: ""
          image: null
      album:
        media_type: album
        uri: library://album/41
        name: The Chopin Project
        version: ""
        image: https://i.scdn.co/image/ab67616d0000b273e64735fbacebe9f1a1519261
        favorite: true
        explicit: null
        discart_image: null
        artists:
          - media_type: artist
            uri: library://artist/39
            name: Ólafur Arnalds
            version: ""
            image: null
          - media_type: artist
            uri: library://artist/40
            name: Alice Sara Ott
            version: ""
            image: null
    stream_title: null
    stream_details:
      provider: spotify--g8pFWq7V
      item_id: 0iBbxZro3fIOks49QfYYfV
      content_type: ogg
      sample_rate: 44100
      bit_depth: 16
      bit_rate: 320
``

## Media player entity attributes
``
volume_level: 0.68
is_volume_muted: false
media_content_type: music
app_id: music_assistant
source: Music Assistant Queue
shuffle: false
repeat: "off"
entity_picture_local: >-
  /api/media_player_proxy/media_player.media_player_tablet_2?token=49459178c8828038a737a4bd3897946bbcff2093ea4633577939aa225f7c63fe&cache=49c21c3dba0ae617
mass_player_type: player
active_queue: media_player.media_player_tablet
device_class: speaker
icon: mdi:speaker
friendly_name: Media Player Tablet
supported_features: 7796287
entity_picture: >-
  http://192.168.178.200:8095/imageproxy?provider=spotify--g8pFWq7V&size=500&fmt=jpeg&path=https%253A//i.scdn.co/image/ab67616d0000b2731e2b495e7e1fd3d3fed3b178
media_content_id: spotify--g8pFWq7V://track/5q7LIsZgR0HfyOBFGGNQnd
media_duration: 180
media_position: 12
media_position_updated_at: "2026-05-31T19:58:13.684445+00:00"
media_title: De chalti Vulkan - Teil 6
media_artist: Kasperli/Nik Hartmann/David Bröckelmann/Fabienne Hadorn/Claudio Zuccolini
media_album_name: De chalti Vulkan / De Förschter Sager und de Holzwurm Drill
media_album_artist: Kasperli
``

# Rules
- Add all the LVGL and UI code at the end of the existing mediaplayertablet.yaml
- Define custom elements (e.g. the entity of the home assistant media player and the local media player) at the beginning of the yaml (as substitutions)

# Documentation
Add a documentation .md file to explain the relevant hardware and software steps to setup this system. The documentation is for non-technical users and should only contain the steps necessary to build and setup the system without unnecessary technical details. It should be possible for a user to follow the documentation and successfully build and setup the system. Do not include any additional code in the documentation, only provide the steps and explanations.
- For the hardware part, describe the required parts and how to connect them (M5Stack Tab5, Unit Scroll and RFID 2, connected by 10 cm grove cables and with a Grove T connector on the RFID 2 module).
- For the software part, describe the configuration of the ESPHome device and the integration into home assistant. Mention that the device has to be allowed to make services calls to the home assistant instance.
- Also describe that the media player (under Home Assistant MediaPlayers) has to be added to music assistant. This musc assistant player is the media player entity which has to be configured in mediaplayertablet.yaml.