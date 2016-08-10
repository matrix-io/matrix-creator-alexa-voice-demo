/*
 * Copyright 2016 <Admobilize>
 * All rights reserved.
 */

#include <wiringPi.h>

#include <string>
#include <iostream>
#include <valarray>
#include <unistd.h>

#include <matrix_hal/everloop_image.h>
#include <matrix_hal/everloop.h>
#include <matrix_hal/microphone_array.h>
#include <matrix_hal/wishbone_bus.h>

#define kIRRxPin 16

namespace hal = matrix_hal;

int main() {
  hal::WishboneBus* bus = new hal::WishboneBus();
  bus->SpiInit();

  hal::MicrophoneArray mics;
  mics.Setup(bus);

  pinMode(kIRRxPin, INPUT);
  pinMode(13, OUTPUT);
  pinMode(5, OUTPUT);

  digitalWrite(13, HIGH);
  digitalWrite(5, HIGH);

  hal::Everloop everloop;
  everloop.Setup(bus);

  hal::EverloopImage image1d;

  for (auto& led : image1d.leds) led.blue = 10;

  everloop.Write(&image1d);

  uint16_t seconds_to_record = 6;

  int16_t buffer[mics.Channels()][seconds_to_record * mics.SamplingRate()];

  uint32_t step = 0;
  while (true) {
    if (!digitalRead(16)) {
      usleep(10);
      if (!digitalRead(16)) break;
    }
  }

  for (auto& led : image1d.leds) {
    led.blue = 0;
    led.red = 10;
  }
  everloop.Write(&image1d);

  while (true) {
    mics.Read(); /* Reading 8-mics buffer from de FPGA */

    for (uint32_t s = 0; s < mics.NumberOfSamples(); s++) {
      for (uint16_t c = 0; c < mics.Channels(); c++) { /* mics.Channels()=8 */
        buffer[c][step] = mics.At(s, c);
      }
      step++;
    }
    if (step == seconds_to_record * mics.SamplingRate()) break;
  }

  uint16_t c = 0;
  std::cout.write((const char*)buffer[c],
                  seconds_to_record * mics.SamplingRate() * sizeof(int16_t));

  std::cout.flush();

  for (auto& led : image1d.leds) {
    led.red = 0;
    led.green = 10;
  }
  everloop.Write(&image1d);
  // }
  digitalWrite(13, LOW);
  digitalWrite(5, LOW);
  return 0;
}
