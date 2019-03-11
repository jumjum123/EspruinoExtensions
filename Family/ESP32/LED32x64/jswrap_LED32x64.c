/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2013 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * ESP32 specific exposed components for LED32x64 boards.
 * ----------------------------------------------------------------------------
 */
#include <stdio.h>

#include "jswrap_LED32x64.h"
#include "anim.h"
#include "rtosutil.h"

#include "esp_system.h"

/*JSON{
 "type"     : "staticmethod",
 "class"    : "LED32x64",
 "name"     : "init",
 "generate" : "jswrap_LED32x64_init"
}*/
void jswrap_LED32x64_init(){
  initImagePartition();
  initI2S();
}

/*JSON{
 "type"     : "staticmethod",
 "class"    : "LED32x64",
 "name"     : "setRGB_addr",
 "generate" : "jswrap_LED32x64_setRGB_addr",
 "params"   : [
   ["rgbAddr", "int", "Adress of RGB data"]
 ]
}*/
void jswrap_LED32x64_setRGB_addr(int rgbAddr){
  anim = rgbAddr;
}

/*JSON{
 "type"     : "staticmethod",
 "class"    : "LED32x64",
 "name"     : "setRGB_Flash",
 "generate" : "jswrap_LED32x64_setRGB_Flash",
 "params"   : [
   ["number", "int", "Number of image in flash"]
 ]
}*/
void jswrap_LED32x64_setRGB_Flash(int number){
  anim = romdata_images + (number * 6144);
}

/*JSON{
 "type"     : "staticmethod",
 "class"    : "LED32x64",
 "name"     : "refresh",
 "generate" : "jswrap_LED32x64_refresh"
}*/
void jswrap_LED32x64_refresh(){
  LED32x64_refresh(0,0);
}