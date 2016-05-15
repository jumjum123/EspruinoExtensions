/*
 * This file is a library for Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2015 Juergen Marsch <juergenmarsch@googlemail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * This file is designed to be parsed during the build process
 *
 * Contains ESP8266 board specific library definitions
 * for handling Neopixel

 * Everything is based on Espruino Source from Neil Kolban
 * and Thorsten van Eicken 
 * ----------------------------------------------------------------------------
 */

/* DO_NOT_INCLUDE_IN_DOCS - this is a special token for common.py,
 so we don't put this into espruino.com/Reference until this is out
 of beta.  */
 
#include "libneoPixel.h"
#include <eagle_soc.h>

#define _BV(bit) (1 << (bit))

static uint32_t _getCycleCount(void) __attribute__((always_inline));

static inline uint32_t _getCycleCount(void) {
  uint32_t ccount;
  __asm__ __volatile__("rsr %0,ccount":"=a" (ccount));
  return ccount;
}

void sendToNeopixel(Pin pin,uint8_t *p,uint8_t *end,uint32_t zeroBitDuration,uint32_t oneBitDuration){
  //uint32_t pinMask = _BV(pin);    // bit mask for GPIO pin to write to reg
  if(!jshGetPinStateIsManual(pin))
    jshPinSetState(pin, JSHPINSTATE_GPIO_OUT);
  p--;
  uint32_t pinMask = 0;
  uint8_t mask = 0x01;  
  uint8_t pix = *p++;             // current byte being shifted out
  //uint8_t mask = 0x80;            // mask for current bit
  uint32_t start;                 // start time of bit
  // iterate through all bits
  ets_intr_lock();                // disable most interrupts
  while(1) {
    uint32_t t;
    //if (pix & mask) t = 56; // one bit, high typ 800ns (56 cyc = 700ns)
    //else            t = 14; // zero bit, high typ 300ns (14 cycl = 175ns)
    if (pix & mask) t = oneBitDuration;
    else            t = zeroBitDuration;
    GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, pinMask);  // Set high
    start = _getCycleCount();                        // get start time of this bit
    while (_getCycleCount()-start < t) ;             // busy-wait
    GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, pinMask);  // Set low
    if (!(mask >>= 1)) {                             // Next bit/byte?
      if (p >= end) break;                           // at end, we're done
      pix = *p++;
      mask = 0x80;pinMask = _BV(pin);
    }
    while (_getCycleCount()-start < 100) ;           // busy-wait, 100 cyc = 1.25us
  }
  while (_getCycleCount()-start < 100) ;             // Wait for last bit
  ets_intr_unlock();

  // at some point the fact that the code above needs to be loaded from flash to cache caused the
  // first bit's timing to be off. If this recurs, a suggestion is to run a loop iteration
  // outputting low-low and only start with the actual first bit in the second loop iteration.
  // This could be achieved by starting with pinMask=0 and setting the real pin mask at the end
  // of the loop, initializing p=pixels-1, and mask=1
}
