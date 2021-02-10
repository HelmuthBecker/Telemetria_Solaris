#!/usr/bin/sh

cp /home/pi/chromium_prefs/Preferences /home/pi/.config/chromium/Default
chromium-browser --kiosk http://localhost:3000/aplicacao
exit 0
