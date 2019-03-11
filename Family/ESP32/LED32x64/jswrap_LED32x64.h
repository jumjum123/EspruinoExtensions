/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2018 Juergen Marsch
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * This file is designed to be parsed during the build process
 *
 * ESP32 specific exposed components for LED32x64 boards.
 * ----------------------------------------------------------------------------
 */
#ifndef TARGETS_LED_JSWRAP_32x64_H_
#define TARGETS_LED_JSWRAP_32x64_H_
#include "jsvar.h"

//===== LED Library
void jswrap_LED32x64_init();
void jswrap_LED32x64_setRGB_addr(int rgbAddr);
void jswrap_LED32x64_setRGB_Flash(int number);
void jswrap_LED32x64_refresh();
#endif /* TARGETS_LED_JSWRAP_32x64_H_ */
