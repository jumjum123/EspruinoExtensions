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
 * Everything is based on Espruino Source from Neil Kolban
 * and Thorsten van Eicken 
 * ----------------------------------------------------------------------------
 */
#ifndef TARGETS_ESP8266_LIBNEOPIXEL_H_
#define TARGETS_ESP8266_LIBNEOPIXEL_H_
#include "jspin.h"
#include "jshardware.h"

void ets_intr_lock(void);
void ets_intr_unlock(void);

void sendToNeopixel(Pin pin,uint8_t *p,uint8_t *end,uint32_t zeroBitDuration,uint32_t oneBitDuration);
 
#endif /* TARGETS_ESP8266_LIBNEOPIXEL_H_ */
