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
 * Contains ESP8266 board specific function definitions
 * for handling LedCircle functions (WS2812 like CJMCU)
 * 
 * Everything is based on Espruino Source from Neil Kolban
 * and Thorsten van Eicken 
 * ----------------------------------------------------------------------------
 */
#ifndef TARGETS_ESP8266_JSWRAP_LEDCIRCLE_H_
#define TARGETS_ESP8266_JSWRAP_LEDCIRCLE_H_
#include "jsvar.h"
#include "jspin.h"
typedef struct {
  int cells,cellsLength;
  int red, green, blue;
  int circleSize[5];
} PACKED_FLAGS JsLedCircleData;
typedef struct JsLedCircle {
  JsVar *LedCircleVar;
  JsLedCircleData data;
} PACKED_FLAGS JsLedCircle;

JsVar *jswrap_LedCircle_createCircleBuffer();
void jswrap_LedCircle_setColor(JsVar *parent, int red, int green, int blue);
void jswrap_LedCircle_setColorHSB(JsVar *parent, JsVarFloat hue, JsVarFloat sat, JsVarFloat bri);
void jswrap_LedCircle_setPixel(JsVar *parent, int circle, int led );
JsVar *jswrap_LedCircle_getPixel(JsVar *parent, int circle, int led);
void jswrap_LedCircle_fill(JsVar *parent);
void jswrap_LedCircle_fillCircle(JsVar *parent,int circle);
JsVar *jswrap_LedCircle_getCircle(JsVar *parent,int circle);
void jswrap_LedCircle_setCircle(JsVar *parent, JsVar *circleBuffer, int circle);
void jswrap_LedCircle_rotate(JsVar *parent, int direction, int circle);

#endif /* TARGETS_ESP8266_JSWRAP_LEDCIRCLE_H_ */
