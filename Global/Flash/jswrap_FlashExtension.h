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
 * 
 * Everything is based on Espruino Source from Gordon Williams
 * ----------------------------------------------------------------------------
 */
#include "jsvar.h"
#include "jswrap_flash.h"

#ifndef TARGETS_ESP8266_JSWRAP_FLASHEXTENSION_H_
#define TARGETS_ESP8266_JSWRAP_FLASHEXTENSION_H_

JsVar *jswrap_flash_readMappedArray(int addr, int pointer);
JsVar *jswrap_flash_readMappedString(int addr, int pointer);
 
#endif /* TARGETS_ESP8266_JSWRAP_FLASHEXTENSION_H_ */
