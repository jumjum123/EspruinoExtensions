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
 * ESP32 specific component for byte port (setting/resetting of 8 pins a time)
 * ----------------------------------------------------------------------------
 */
#ifndef TARGETS_BYTEPORT_H_
#define TARGETS_BYTEPORT_H_
#include "jsvar.h"
#include "jspin.h"

//===== BytePort Library
#define jswrap_byteport_pins 8

typedef struct {
  int mask[jswrap_byteport_pins];
  Pin clk;
  bool clkPol; // clock polarity
  Pin enable;
  bool enablePol;  
} PACKED_FLAGS JsBytePortData;
typedef struct {
  JsVar *BytePortVar;
  JsBytePortData data;
} PACKED_FLAGS JsBytePort;

void bytePortInit(JsVar *parent,JsBytePort *bp);
void bytePortWrite(JsVar *parent,int value);
void bytePortWriteArray(JsVar *parent,uint8_t *dataPnt,size_t len);

JsVar *jswrap_BytePort_constructor(JsVar *pins);
int jswrap_BytePort_getMask(JsVar *parent,int value);
void jswrap_BytePort_setValue(JsVar *parent,int value);
void jswrap_BytePort_setValues(JsVar *parent, JsVar *data);
void jswrap_BytePort_setClock(JsVar *parent,Pin clk,bool polarity);
void jswrap_BytePort_setEnable(JsVar *parent,Pin enable,bool polarity);
#endif /* TARGETS_BYTEPORT_H_ */
