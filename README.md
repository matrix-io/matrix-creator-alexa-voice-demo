
### Alexa Running on MATRIX Creator/Voice + Raspberry Pi 3###

 [![Amazon's Alexa with MATRIX Creator + Raspberry
](http://img.youtube.com/vi/8iUDyo5sQJI/0.jpg)](https://www.youtube.com/watch?v=8iUDyo5sQJI)

### MATRIX MALOS service

install MALOS and perform device reboot. For more details: [Getting Started Guide](https://github.com/matrix-io/matrix-creator-quickstart/wiki/2.-Getting-Started)

``` bash
echo "deb http://packages.matrix.one/matrix-creator/ ./" | sudo tee --append /etc/apt/sources.list
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install matrix-creator-init matrix-creator-malos cmake g++ git libzmq3-dev --no-install-recommends
reboot

```

This project was based in https://github.com/novaspirit/AlexaPi

Detailed recipe published in Instructables http://www.instructables.com/id/Build-a-DIY-Amazons-Alexa-With-a-Raspberry-Pi-and-/


