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
 * for handling LedMatrix functions (WS2812)
 * 
 * Everything is based on Espruino Source from Neil Kolban
 * and Thorsten van Eicken 
 * ----------------------------------------------------------------------------
 */
#ifndef TARGETS_ESP8266_JSWRAP_LEDMATRIX_H_
#define TARGETS_ESP8266_JSWRAP_LEDMATRIX_H_
#include "jsvar.h"
#include "jspin.h"
typedef struct {
  int width,height,cells,cellsLength;
  int red,green,blue;
  uint32_t oneBitDuration, zeroBitDuration;
  Pin pin;
} PACKED_FLAGS JsLedMatrixData;
typedef struct JsLedMatrix {
  JsVar *LedMatrixVar;
  JsLedMatrixData data;
} PACKED_FLAGS JsLedMatrix;

JsVar *jswrap_LedMatrix_createArrayBuffer(int width,int height,Pin pin);
void jswrap_LedMatrix_show(JsVar *parent);
void jswrap_LedMatrix_setPulseLength(JsVar *parent, int zeroBitDuration, int oneBitDuration);
void jswrap_LedMatrix_setColor(JsVar *parent, int red, int green, int blue);
void jswrap_LedMatrix_setColorHSB(JsVar *parent, JsVarFloat hue, JsVarFloat sat, JsVarFloat bri);
void jswrap_LedMatrix_setPixel(JsVar *parent, int row, int column );
JsVar *jswrap_LedMatrix_getPixel(JsVar *parent, int row, int column);
void jswrap_LedMatrix_fill(JsVar *parent);
void jswrap_LedMatrix_drawColumn(JsVar *parent, int column);
void jswrap_LedMatrix_drawRow(JsVar *parent, int row);
JsVar *jswrap_LedMatrix_getRow(JsVar *parent,int row);
void jswrap_LedMatrix_setRow(JsVar *parent, JsVar *rowBuffer, int row);
JsVar *jswrap_LedMatrix_getColumn(JsVar *parent,int column);
void jswrap_LedMatrix_setColumn(JsVar *parent, JsVar *columnBuffer, int column);
void jswrap_LedMatrix_shift(JsVar *parent, int direction);
void jswrap_LedMatrix_rotate(JsVar *parent, int direction);

#endif /* TARGETS_ESP8266_JSWRAP_LEDMATRIX_H_ */
