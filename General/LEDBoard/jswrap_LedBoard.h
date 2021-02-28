/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2021 Juergen Marsch
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * This file is designed to be parsed during the build process
 *
 * General functions to handle LED Matrix Boards
 * ----------------------------------------------------------------------------
 */
#ifndef TARGETS_JSWRAP_LEDBOARD_H_
#define TARGETS_JSWRAP_LEDBOARD_H_

#include "jsvar.h"
#include "jspin.h"

#define POSTREFRESH_CALLBACK_NAME JS_EVENT_PREFIX"postRefresh"
//Led Board functions
void jswrap_LedBoard_init(JsVar *dataPins,JsVar *addressPins,JsVar *controlPins);
void jswrap_LedBoard_send(JsVar *graphics);
void jswrap_LedBoard_activateEvent(bool status);
void jswrap_LedBoard_rotate(JsVar *graphics, int direction,JsVar *frame);
void jswrap_LedBoard_reColor(JsVar *graphics,JsVar *colors,JsVar *frame);
JsVar *jswrap_LedBoard_getFrame(JsVar *graphics,JsVar *frame);
void jswrap_LedBoard_setFrame(JsVar * graphics, JsVar *frame, JsVar *frameData);

//move this to c-File
int addressPinsLength;
int pixelRows; //will be calculated during init based number of adrPins
int pixelsInRow; //will be calculated during send by size of buffer(???)

#ifdef LEDBOARD_DEBUG
//Testing functions
void jswrap_LedBoard_debug(JsVar *graphics);
void jswrap_LedBoard_debugInfo();
void jswrap_LedBoard_testAddress(int repeat);
void jswrap_LedBoard_testRGBData(JsVar *graphics,int repeat);
void jswrap_LedBoard_testPin(Pin pin,bool value);
void jswrap_LedBoard_pulsePin(Pin pin,bool value);
void jswrap_LedBoard_setPixel(JsVar *graphics,int x, int y, int color);
void jswrap_LedBoard_getPixel(JsVar *graphics,int x, int y);
#endif
//Controler specific functions
bool LedBoardSaveDataPin(Pin pin,int i);
bool LedBoardSaveAddressPin(Pin pin,int i);
bool LedBoardSaveControlPin(Pin pin,int i);
void LedBoardSetAddress(int row);
void LedBoardSetEnable(bool value);
void LedBoardPulseLatch();
void LedBoardSetDefault();
void LedBoardSendRow(uint8_t *dataPnt);
#ifdef LEDBOARD_DEBUG
//Controler specific testing functions
void LedBoardDebugInfo();
void LedBoardtestPin(Pin pin,bool value);
void LedBoardpulsePin(Pin pin,bool value);
#endif
#endif /* TARGETS_JSWRAP_LEDBOARD_H_ */
