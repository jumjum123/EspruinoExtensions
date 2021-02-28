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

volatile uint32_t *r1Address;
volatile uint32_t *r2Address;
volatile uint32_t *g1Address;
volatile uint32_t *g2Address;
volatile uint32_t *b1Address;
volatile uint32_t *b2Address;

volatile uint32_t *AAddress;
volatile uint32_t *BAddress;
volatile uint32_t *CAddress;
volatile uint32_t *DAddress;
volatile uint32_t *EAddress;
volatile uint32_t *enableAddress;
volatile uint32_t *latchAddress;
volatile uint32_t *clockAddress;

bool LedBoardSaveDataPin(Pin pin,int i){
  switch(i){
    case 0: r1Address = jshGetPinAddress(pin, JSGPAF_OUTPUT); break;
    case 1: r2Address = jshGetPinAddress(pin, JSGPAF_OUTPUT); break;
    case 2: g1Address = jshGetPinAddress(pin, JSGPAF_OUTPUT); break;
    case 3: g2Address = jshGetPinAddress(pin, JSGPAF_OUTPUT); break;
    case 4: b1Address = jshGetPinAddress(pin, JSGPAF_OUTPUT); break;
    case 5: b2Address = jshGetPinAddress(pin, JSGPAF_OUTPUT); break;
  }
  return true;  
}
bool LedBoardSaveAddressPin(Pin pin,int i){
  switch(i){
    case 0: AAddress = jshGetPinAddress(pin, JSGPAF_OUTPUT);break;
    case 1: BAddress = jshGetPinAddress(pin, JSGPAF_OUTPUT);break;
    case 2: CAddress = jshGetPinAddress(pin, JSGPAF_OUTPUT);break;
    case 3: DAddress = jshGetPinAddress(pin, JSGPAF_OUTPUT);break;	  
    case 4: EAddress = jshGetPinAddress(pin, JSGPAF_OUTPUT);break;
  }
  return true;
}
bool LedBoardSaveControlPin(Pin pin,int i){
  switch(i){
    case 0: enableAddress = jshGetPinAddress(pin, JSGPAF_OUTPUT); break;
    case 1: latchAddress = jshGetPinAddress(pin, JSGPAF_OUTPUT); break;
    case 2: clockAddress = jshGetPinAddress(pin, JSGPAF_OUTPUT); break;
  }
  return true;
}
void LedBoardSetDefault(){
  *r1Address = 0;
  *r2Address = 0;
  *b1Address = 0;
  *b2Address = 0;
  *g1Address = 0;
  *g2Address = 0;
  *enableAddress = 0;
  *latchAddress = 0;
  *clockAddress = 1;
  *AAddress = 0;
  *BAddress = 0;
  *CAddress = 0;
  *DAddress = 0;
  *EAddress = 0;
}
void LedBoardSetEnable(bool value){
  *enableAddress = value;
}
void LedBoardPulseLatch(){
  *latchAddress = true;
  *latchAddress = false;
}
void LedBoardPulseClock(){
  *clockAddress = false;
  *clockAddress = true;
}
void LedBoardSetAddress(int row){
  int i;
  for(i = 0; i < addressPinsLength; i++){
    switch(i){
      case 0: *AAddress = (row & 1); break; 
      case 1: *BAddress = (row & 1); break; 
      case 2: *CAddress = (row & 1); break; 
      case 3: *DAddress = (row & 1); break; 
      case 4: *EAddress = (row & 1); break; 
    }
    row>>=1;	
  }
}
void LedBoardSendRow(uint8_t *dataPnt){
  int i,val;
  for(i = 0; i < pixelsInRow; i++){
    val = *dataPnt;
    *b1Address = (val & 1);val>>=1;
	*g1Address = (val & 1);val>>=1;
	*r1Address = (val & 1);val>>=2;
	*b2Address = (val & 1);val>>=1;
	*g2Address = (val & 1);val>>=1;
	*r2Address = (val & 1);
	dataPnt++;
	LedBoardPulseClock();
  }
}
#ifdef LEDBOARD_DEBUG
void LedBoardtestPin(Pin pin,bool value){
  uint32_t *pinAddress;
  pinAddress = jshGetPinAddress(pin, JSGPAF_OUTPUT);
  jsWarn("Mask:%d",pinAddress);
  *pinAddress = value;
}
void LedBoardpulsePin(Pin pin,bool value){
  uint32_t *pinAddress;
  pinAddress = jshGetPinAddress(pin, JSGPAF_OUTPUT);
  jsWarn("PulseMask:%d",pinAddress);
  *pinAddress = value;
  *pinAddress = !value;  
}
void LedBoardDebugInfo(){
  jsWarn("adrLength:%d,rows:%d,pixelsInRow:%d",addressPinsLength,pixelRows,pixelsInRow);
  jsWarn("Address:%d,%d,%d,%d,%d",AAddress,BAddress,CAddress,DAddress,EAddress);
  jsWarn("Data:%d,%d,%d,%d,%d,%d",r1Address,r2Address,g1Address,g2Address,b1Address,b2Address);
  jsWarn("enable:%d,latch:%d,clock:%d",enableAddress,latchAddress,clockAddress);
}
#endif