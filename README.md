# PicoWLEDStrip
FreeRTOS, LWIP, MQTT and NeoPixel control for custom cabinet lighting

This is very much work-in-progress code.  The desired end point here is that a 'squadron' of Pico W devices will all talk to a Home Assistant node via MQTT.  Each Pico W will have one (or maybe two) strips of Neopixels attached, which will be used to provide lighting inside some glass fronted Ikea cabinets.  Home Assistant integration will allow for remote operation of the strips, and hopefully for automated modulation of the colour of the lighting throught the 24 hour period - ideally the colour of the lighting will align with the day/night cycle, so for example at dusk there will be nice warm colours, at noon the light will be nice and bright.   
