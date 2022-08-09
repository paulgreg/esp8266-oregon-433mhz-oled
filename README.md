# ESP8266 temperature receiver

That project is using an esp8266, an RF module 433Mhz receiver and an OLED screen to get temperature from an Oregon Scientific sensor, to display it on an OLED screen and to send it on the web.

It uses the great [ErriezOregonTHN128](https://github.com/Erriez/ErriezOregonTHN128) library to fetch and decode Oregon Scientific radio data.

Here’s the [hackaday project page](https://hackaday.io/project/171910-esp32-weather-station) with more details and pictures.


## Arduino env

use Board "Wemos D1 mini" to build with Arduino IDE.

Copy `parameters.h.dist` to `parameters.h` and update it with your wifi settings.

The data is posted to a simple server used to save JSON : [json-store](https://github.com/paulgreg/json-store)