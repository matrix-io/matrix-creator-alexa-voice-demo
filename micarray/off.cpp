/*
 * Copyright 2016 <Admobilize>
 * All rights reserved.
 */

#include <string>
#include <iostream>
#include <valarray>
#include <unistd.h>

#include <matrix_hal/everloop_image.h>
#include <matrix_hal/everloop.h>
#include <matrix_hal/wishbone_bus.h>

namespace hal = matrix_hal;

int main() {
  hal::WishboneBus* bus = new hal::WishboneBus();
  bus->SpiInit();


  hal::Everloop everloop;
  everloop.Setup(bus);

  hal::EverloopImage image1d;

  for (auto& led : image1d.leds) led.blue = 10;

  everloop.Write(&image1d);

  return 0;
}
