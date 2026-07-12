# Bark Detector

A simple sound peak detector using the [M5 atom echo smart speaker development kit](https://shop.m5stack.com/products/atom-echo-smart-speaker-dev-kit)

When a sound peak of a specified threshold is detected, an endpoint is called. In conjunction with something like Home Assistant and NodeRed, one can then setup a system to notify of loud sounds when nobody is home.

## Usage

Copy `Credentials.example.h` to `loud_sound/Credentials.h` and fill in the appropriate values for your WiFi network and server endpoints. Use Arduino IDE to flash the code to the [M5 Atom Echo](https://docs.m5stack.com/en/arduino/m5atomecho/program), using the M5Unified library.
