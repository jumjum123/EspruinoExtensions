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
#include "nrf_gpio.h"

int r1Mask;
int r2Mask;
int g1Mask;
int g2Mask;
int b1Mask;
int b2Mask;

int AMask;
int BMask;
int CMask;
int DMask;
int EMask;
int enableMask;
int latchMask;
int clockMask;

NRF_GPIO_Type *IOreg;
function getIOReg(){
  uint32_t defaultPort = 15
  reg = nrf_gpio_pin_port_decode(&defaultPort);
}
bool LedBoardSaveDataPin(Pin pin,int i){
  bool r;
  if(pin < 32){
    switch(i){
	  case 0: r1Mask = 1<<pin;break;	
	  case 1: r2Mask = 1<<pin;break;	
	  case 2: g1Mask = 1<<pin;break;	
	  case 3: g2Mask = 1<<pin;break;	
	  case 4: b1Mask = 1<<pin;break;	
	  case 5: b2Mask = 1<<pin;break;	
	}
    r = true;
  }
  else r = false;
  return r;
}
bool LedBoardSaveAddressPin(Pin pin,int i){
  bool r;
  if(pin < 32){
    switch(i){
      case 0: AMask = 1<<pin;break;
      case 1: BMask = 1<<pin;break;
      case 2: CMask = 1<<pin;break;
      case 3: DMask = 1<<pin;break;
      case 4: EMask = 1<<pin;break;
	}
	r = true;
  }
  else r = false;
  return r;
}
bool LedBoardSaveControlPin(Pin pin,int i){
  bool r;
  if(pin < 32){
	switch(i){
	  case 0: enableMask = 1<<pin; break;
	  case 1: latchMask = 1<<pin; break;
	  case 2: clockMask = 1<<pin; break;
	}
	r = true;
  }
  else r = false;
  return r;
}
void LedBoardSetDefault(){
  r1Mask = 0;
  r2Mask = 0;
  b1Mask = 0;
  b2Mask = 0;
  g1Mask = 0;
  g2Mask = 0;
  enableMask = 0;
  latchMask = 0;
  clockMask = 0;
  AMask = 0;
  BMask = 0;
  CMask = 0;
  DMask = 0;
  EMask = 0; 
  getIOReg();  
}

void LedBoardSetEnable(bool value){
  if(value) reg->OUTSET = enableMask;
  else reg->OUTCLR = enableMask;
}
void LedBoardPulseLatch(){
  reg->OUTSET = latchMask;
  reg->OUTCLR = latchMask;
}
void LedBoardPulseClock(){
  reg->OUTCLR = clockMask;
  reg->OUTSET = clockMask;
}
void LedBoardSetAddress(int row){
  int i;long resetMask,setMask;
  resetMask = 0;setMask = 0;
  for(i = 0; i < addressPinsLength; i++){
    switch(i){
      case 0: if(row & 1) setMask += AMask; else resetMask += AMask; break;
      case 1: if(row & 1) setMask += BMask; else resetMask += BMask; break;
      case 2: if(row & 1) setMask += CMask; else resetMask += CMask; break;
      case 3: if(row & 1) setMask += DMask; else resetMask += DMask; break;
      case 4: if(row & 1) setMask += EMask; else resetMask += EMask; break;
	}
    row>>=1;
  }
  reg->OUTCLR = resetMask;
  reg->OUTSET = setMask;
}
void LedBoardSendRow(uint8_t *dataPnt){
  int i,val,resetMask,setMask;
  for(i = 0; i < pixelsInRow; i++){
	resetMask = 0;
	setMask = 0;
    val = *dataPnt;
	if(val & 1) setMask += b1Mask; else resetMask += b1Mask;
	val>>=1;
	if(val & 1) setMask += g1Mask; else resetMask += g1Mask;
	val>>=1;
	if(val & 1) setMask += r1Mask; else resetMask += r1Mask;
	val>>=2;
	if(val & 1) setMask += b2Mask; else resetMask += b2Mask;
	val>>=1;
	if(val & 1) setMask += g2Mask; else resetMask += g2Mask;
	val>>=1;
	if(val & 1) setMask += r2Mask; else resetMask += r2Mask;
	reg->OUTCLR = resetMask;
	reg->OUTSET = setMask;
	dataPnt++;
	LedBoardPulseClock();
  }
	
}
void LedBoardtestPin(Pin pin,bool value){
  int pinMask;
  pinMask = 1<<pin;
  if(value) reg->OUTSET = pinMask; else reg->OUTCLR = pinMask;
  jsWarn("Mask:%d",pinMask);
}
void LedBoardpulsePin(Pin pin,bool value){
  int pinMask;
  pinMask = 1<<pin;
  if(value) reg->OUTSET = pinMask;else reg->OUTCLR = pinMask;
  jsWarn("PulseMask:%d",pinMask);
  if(value) reg->OUTCLR = pinMask;else reg->OUTSET = pinMask;  
}
void LedBoardDebugInfo(){
  jsWarn("adrLength:%d,rows:%d,pixelsInRow:%d",addressPinsLength,pixelRows,pixelsInRow);
  jsWarn("Address:%d,%d,%d,%d,%d",AMask,BMask,CMask,DMask,EMask);
  jsWarn("Data:%d,%d,%d,%d,%d,%d",r1Mask,r2Mask,g1Mask,g2Mask,b1Mask,b2Mask);
  jsWarn("enable:%d,latch:%d,clock:%d",enableMask,latchMask,clockMask);
}
