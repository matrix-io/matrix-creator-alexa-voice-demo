#!/bin/bash

sudo apt install mpg123 libasound2-dev

# python
sudo apt install libpython-dev python-alsaaudio
sudo pip install -r requirements.txt

# alsa
cp rc/asoundrc ${HOME}/.asoundrc


# sensory
git submodule init
git submodule update

third_party/alexa-rpi/bin/license.sh


sudo cp third_party/alexa-rpi/lib/libsnsr.a /usr/local/lib
sudo cp third_party/alexa-rpi/include/*.h /usr/local/include
sudo mkdir /usr/local/resources/
sudo cp third_party/alexa-rpi/models/*.snsr /usr/local/resources
sudo cp third_party/alexa-rpi/models/spot-alexa-rpi-31000.snsr /usr/local/resources/spot-alexa-rpi.snsr

cd  wakeword && g++ main.cpp alexa_wakeword.cpp -I. -std=c++11 -lsnsr -lasound -lpthread -o wake_word
