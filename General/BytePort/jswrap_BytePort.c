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

static inline void structInit(JsBytePort *bp, JsVar *pins){
  int i,v,cnt = 0; Pin pin;
  JsvObjectIterator it;
  if (jsvIsArray(pins)){
    for(i = 0; i < jswrap_byteport_pins; i++) bp->data.mask[i] = 0;
    jsvObjectIteratorNew(&it, pins);
    while (jsvObjectIteratorHasValue(&it)) {
      if (cnt >= jswrap_byteport_pins) {
        jsExceptionHere(JSET_ERROR, "Too many pins! %d Maximum.", jswrap_byteport_pins);
        return;
      }
      pin = jshGetPinFromVarAndUnLock(jsvObjectIteratorGetValue(&it));
      if (jshIsPinValid(pin)) {
        if (!jshGetPinStateIsManual(pin)){
          jshPinSetState(pin, JSHPINSTATE_GPIO_OUT);
          bp->data.mask[cnt] = 1<<pin;
        }
      }
      cnt++;
      jsvObjectIteratorNext(&it);
    }
    jsvObjectIteratorFree(&it);
  }
  else jsExceptionHere(JSET_ERROR, "not an array of pins");
}
bool getFromVar(JsBytePort *bp, JsVar *parent){
  bp->BytePortVar = parent;
  JsVar *data = jsvObjectGetChild(parent,JS_HIDDEN_CHAR_STR"bp",0);
  assert(data);
  if(data){
    jsvGetString(data, (char*)&bp->data, sizeof(JsBytePortData)+1);
    jsvUnLock(data);
    return true;
  } else
    return false;
}
void setVar(JsBytePort *bp){
  JsVar *dataname = jsvFindChildFromString(bp->BytePortVar,JS_HIDDEN_CHAR_STR"bp",true);
  JsVar *data = jsvSkipName(dataname);
  if (!data) {
    data = jsvNewStringOfLength(sizeof(JsBytePortData),NULL);
    jsvSetValueOfName(dataname, data);
  }
  jsvUnLock(dataname);
  assert(data);
  jsvSetString(data, (char*)&bp->data, sizeof(JsBytePortData));
  jsvUnLock(data);
}

/*JSON{
  "type"  : "class",
  "class" : "BytePort"
}
This is the built-in class for Byte Port
*/
/*JSON{
  "type"     : "constructor",
  "class"    : "BytePort",
  "name"     : "BytePort",
  "generate" : "jswrap_BytePort_constructor",
  "params"   : [
    ["pins","JsVar","An array of pins to use"],
	["clock","JsVar","Pin for Clock Signal"],
	["polarity","bool","Polarity of Clock Signal"]
  ],
  "return"   : ["JsVar","A BytePort object"]
}
*/
JsVar *jswrap_BytePort_constructor(JsVar *pins){
  JsVar *parent = jspNewObject(0,"BytePort");
  //jsvObjectSetChild(parent,"PortMask",jsvNewNull());
  JsBytePort bp;
  structInit(&bp,pins);
  bp.BytePortVar = parent;
  bytePortInit(parent,&bp);
  setVar(&bp);
  return parent;
}

/*JSON{
  "type"     : "method",
  "class"    : "BytePort",
  "name"     : "getMask",
  "generate" : "jswrap_BytePort_getMask",
  "params"   : [
    ["value","int","value"]
  ],
  "return"   : ["int","portmask"]
}
*/
int jswrap_BytePort_getMask(JsVar *parent,int value){
  JsBytePort bp;int i,mask;
  if(!getFromVar(&bp,parent)) return 0;
  mask = 0;
  for(i = 0; i < jswrap_byteport_pins; i++){
	if((value & 1)>0) mask += bp.data.mask[i];
    value = value>>1;
  }
  return mask;
}

/*JSON{
  "type"     : "method",
  "class"    : "BytePort",
  "name"     : "setValue",
  "generate" : "jswrap_BytePort_setValue",
  "params"   : [
    ["value", "int", "Value for ports belonging to byte"]
  ] 
}
Set all ports (pins) of a byte at once
 */	
void jswrap_BytePort_setValue(JsVar *parent,int value){
  JsBytePort bp;
  if(!getFromVar(&bp,parent)) return;
  JsBytePortData bpd = bp.data;
  if(jshIsPinValid(bpd.enable)){
	jshPinSetValue(bpd.enable, bpd.enablePol);
  }
  bytePortWrite(parent,value);
  if(jshIsPinValid(bpd.enable)){
	jshPinSetValue(bpd.enable, !bpd.enablePol);
  }
}

/*JSON{
  "type"     : "method",
  "class"    : "BytePort",
  "name"     : "setValues",
  "generate" : "jswrap_BytePort_setValues",
  "params"   : [
    ["data", "JsVar", "dataArray of values for ports belonging to byte"]
  ] 
}
Set all ports (pins) of a byte at once
 */
void jswrap_BytePort_setValues(JsVar *parent,JsVar *data) {
  JsBytePort bp;
  if(!getFromVar(&bp,parent)) return;
  JsBytePortData bpd = bp.data;
  uint8_t *dataPnt = jswrap_espruino_getAddressOf(data,true);
  size_t len = jsvGetArrayBufferLength(data);
  bytePortWriteArray(parent,dataPnt,len);
  if (jshIsPinValid(bpd.clk)) {
    jshPinSetValue(bpd.clk, bpd.clkPol);
    jshPinSetValue(bpd.clk, !bpd.clkPol);
  }
}
 
/*JSON{
  "type"     : "method",
  "class"    : "BytePort",
  "name"     : "setClock",
  "generate" : "jswrap_BytePort_setClock",
  "params"   : [
    ["clock", "pin", "port to be used for clock"],
	["polarity", "bool", "polarity of clock signal"]
  ] 
}
Set pin and polarity for clock signal(used by setValues) after each byte
 */	
void jswrap_BytePort_setClock(JsVar *parent,Pin clk,bool polarity){
  JsBytePort bp;
  if(!getFromVar(&bp,parent)) return;
  bp.data.clk = clk;
  bp.data.clkPol = polarity;
  setVar(&bp);
}

/*JSON{
  "type"     : "method",
  "class"    : "BytePort",
  "name"     : "setEnable",
  "generate" : "jswrap_BytePort_setEnable",
  "params"   : [
    ["enable", "pin", "port to be used for enable"],
	["polarity", "bool", "polarity of enable signal"]
  ] 
}
Set pin for 
*/
void jswrap_BytePort_setEnable(JsVar *parent,Pin enable,bool polarity){
  JsBytePort bp;
  if(!getFromVar(&bp,parent)) return;
  bp.data.enable = enable;
  bp.data.enablePol = polarity;
  setVar(&bp);
}