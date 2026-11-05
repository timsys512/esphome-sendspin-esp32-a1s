ESP32-A1S Line 1 Media Source Bridge

1. Copy this entire folder into /config/esphome/a1s_line_in_package/.
2. Keep the YAML and the components directory together.
3. Validate the YAML in ESPHome before flashing.
4. In Home Assistant, select Line 1 by playing URI: line-in://line1
5. Stop the player before switching back to Sendspin or Bluetooth.

This bridge captures 16-bit stereo PCM from the ES8388 LINE1 ADC through
line1_input and forwards it to speaker_source using ESPHome's MediaSource API.
