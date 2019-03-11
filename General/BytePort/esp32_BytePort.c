/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2018 Juergen Marsch <jumjum123>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * ESP32 specific component for byte port (setting/resetting of 8 pins a time)
 * ----------------------------------------------------------------------------
 */
#include "jswrap_BytePort.h"

#include <stdio.h>
#include "esp_system.h"

#include "jswrap_arraybuffer.h"
#include "jsparse.h"
#include "jswrap_object.h"
#include "jshardware.h"
#include "jswrap_espruino.h"
#include "jsutils.h"

void bytePortInit(JsVar *parent,JsBytePort *bp){
  int i,j,value;
  uint32_t mask;
  JsVar *buf = jswrap_typedarray_constructor(ARRAYBUFFERVIEW_UINT32, jsvNewFromInteger(256),0,0);
  uint32_t *maskPnt = (uint32_t *)jswrap_espruino_getAddressOf(buf,true);
  for(int i = 0; i < 256;i++){
    mask = 0;
	value = i;
    for(j = 0; j < jswrap_byteport_pins; j++){
	  if((value & 1)>0) mask += bp->data.mask[j];
      value = value>>1;
    }
    maskPnt[i] = mask;	
  }
  jsvObjectSetChild(parent,"PortMask",buf);
  jsvUnLock(buf);  
}
void bytePortWriteArray(JsVar *parent,uint8_t *dataPnt,size_t len){
  JsVar *portMask = jsvObjectGetChild(parent,"PortMask",0);
  uint32_t *maskPnt = (uint32_t *)jswrap_espruino_getAddressOf(portMask,true);
  uint32_t clearMask = maskPnt[255];
  for(int i = 0; i < len; i++){
	GPIO.out_w1tc = clearMask;
	GPIO.out_w1ts = maskPnt[*dataPnt];
	dataPnt++;
  }
  jsvUnLock(portMask);  
}
void bytePortWrite(JsVar *parent,int value){
  JsVar *portMask = jsvObjectGetChild(parent,"PortMask",0);
  uint32_t *maskPnt = (uint32_t *)jswrap_espruino_getAddressOf(portMask,true);
  uint32_t clearMask = maskPnt[255];
  GPIO.out_w1tc = clearMask;
  GPIO.out_w1ts = maskPnt[value]; 
  jsvUnLock(portMask);  
}
