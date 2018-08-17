Aerie - An open source ESP8266/Arduino based thermostat

There are many examples of creating a thermostat for ESP8266 and Arduino. I hope this project will be able to utilize the efforts of everyone into one great project using all the best practices available.

Currently the project has basic support for ESP8266WebServer, DHT11, and a SSED1306 LCD.

Future:
Need to implement in SPIFFS to have a local copy of the set point and possibly other values.

Need to implement MQTT to interface with homeautomation and NodeRed

Design Thoughts:
I thought of a Nextion LCD panel, but are you really going to be hanging out at the wall very much? I would imagine you would mostly be controlling this from your phone, so having a complicated wall display is frivolous. The 1306 or something simliarly basic should be sufficient.
