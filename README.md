# KevinControl
This project realizes a remote control for some components of my van Kevin.

It consists out of following components:
- Remote server (Walter IOT)
- Flutter App
- Battery control ESP32 (read battery states and control outputs)
- ...

The components are communicating with the remote server via ESP-NOW.
The remote server has a bluetooth interface that can be accessed via a flutter app.
