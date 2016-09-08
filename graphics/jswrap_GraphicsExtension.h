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
 * Everything is based on Espruino Source from Gordon Williams
 * ----------------------------------------------------------------------------
 */
#include "jsvar.h"
#include "graphics.h"

#ifndef TARGETS_ESP8266_JSWRAP_GRAPHICSEXTENSION_H_
#define TARGETS_ESP8266_JSWRAP_GRAPHICSEXTENSION_H_

void jswrap_graphics_drawCircle(JsVar *parent, int x, int y, int radius);
void jswrap_graphics_fillCircle(JsVar *parent, int x, int y, int radius);

#endif /* TARGETS_ESP8266_JSWRAP_GRAPHICSEXTENSION_H_ */
