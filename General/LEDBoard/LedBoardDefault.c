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
 * Controler board specific functions to handle LED Matrix Boards
 * ----------------------------------------------------------------------------
*/

#include "jswrap_LedBoard.h"
#include "jsparse.h"
#include "jswrap_arraybuffer.h"
#include "jswrap_espruino.h"
#include "jsinteractive.h"

Pin dataPins[6];
Pin addressPins[5];
Pin enablePin;
Pin latchPin;
Pin clockPin;

bool LedBoardSaveDataPin(Pin pin,int i){
  dataPins[i] = pin;
  return true;  
}
bool LedBoardSaveAddressPin(Pin pin,int i){
  addressPins[i] = pin;
  return true;
}
bool LedBoardSaveControlPin(Pin pin,int i){
  switch(i){
    case 0: enablePin = pin; break;
    case 1: latchPin = pin; break;
    case 2: clockPin = pin; break;
  }
  return true;
}
void LedBoardSetEnable(bool value){
  jshPinSetValue(enablePin,value);
}
void LedBoardPulseLatch(){
  jshPinSetValue(pulsePin,true);
  jshPinSetValue(pulsePin,false);
}
void LedBoardPulseClock(){
  jshPinSetValue(clockPin,true);
  jshPinSetValue(clockPin,false);
}
void LedBoardSetAddress(int row){
  int i;
  for(i = 0; i < addressPinsLength; i++){
    jshPinSetValue(addressPins[i],row & 1);
    row>>=1;	
  }
}
void LedBoardSendRow(uint8_t *dataPnt){
  int i,val;
  for(i = 0; i < pixelsInRow; i++){
    val = *dataPnt;
    jshPinSetValue(dataPin[0], val & 1);
	jshPinSetValue(dataPin[1], val & 16);
	jshPinSetValue(dataPin[2], val & 2);
	jshPinSetValue(dataPin[3], val & 32);
	jshPinSetValue(dataPin[4], val * 4);
	jshPinSetValue(dataPin[5], val & 64);
	LedBoardPulseClock();
	dataPnt++;
  }
}
void LedBoardtestPin(Pin pin,bool value){
  jshPinSetValue(pin,value);
}
void LedBoardpulsePin(Pin pin,bool value){
  jshPinSetValue(pin,value);
  jshPinSetValue(pin,!value); 
}