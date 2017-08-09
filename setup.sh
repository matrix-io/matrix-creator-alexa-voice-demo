#!/bin/bash

third_party/alexa-rpi/bin/license.sh

cp third_party/alexa-rpi/lib/libsnsr.a /usr/local/lib
cp third_party/alexa-rpi/include/*.h /usr/local/include
mkdir /usr/local/resources/
cp third_party/alexa-rpi/models/*.snsr /usr/local/resources
cp third_party/alexa-rpi/models/spot-alexa-rpi-31000.snsr /usr/local/resources/spot-alexa-rpi.snsr

