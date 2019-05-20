/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2019 Juergen Marsch <juergenmarsch@googlemail.com>
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
#include "jswrap_Gates.h"
#include "jsparse.h"
#include "jsinteractive.h"
#include "jsutils.h" 
#include "jshardware.h"

#define DigitalGateID "DigitalGate"
#define DigitalGateType "type"
#define DigitalGateArgs "args"
#define DigitalGateOut "out"
#define DigitalGateEdge "edge"
#define DigitalGateRaise "raise"
#define DigitalGateFall "fall"
#define DigitalGateInputArr "inp"
#define DigitalGateInputSet "set"
#define DigitalGateInputReset "reset"
#define DigitalGateInputValue "values"
#define DigitalGateInputLength "length"
#define DigitalGateInputPnt "pnt"
#define DigitalGateInputCntmax "cnt"

bool Gates_getPortValue(JsVar *parent,JsVar *value){
  if(jsvIsBoolean(value)) return jsvGetBool(value);
  if(jsvIsPin(value)) return (bool)jshPinGetValue(value);
  if(jsvIsNumeric(value)) return (bool)jsvGetInteger(value);
  if(jsvIsFunction(value)){
	JsVar *args[1];
	args[0] = parent;
    return jsvGetBool(jspeFunctionCall(value, 0, 0, false, 1, args));
  }
  if(jsvIsObject(value)){
	if(jsvIsInstanceOf(value,DigitalGateID)){
      JsVar *val = jsvObjectGetChild(value,DigitalGateOut,0);
	  return jsvGetBool(val);
    }	
  }
  return false;
}
bool Gates_AND(JsVar *parent,JsVar *inputArr){
  bool r = true;bool b;
  JsvIterator itsrc;
  jsvIteratorNew(&itsrc, inputArr, JSIF_EVERY_ARRAY_ELEMENT);
  while (jsvIteratorHasElement(&itsrc)) {
	b = Gates_getPortValue(parent,jsvIteratorGetValue(&itsrc));
	if(b == false) r = false;
    jsvIteratorNext(&itsrc);
  }
  jsvIteratorFree(&itsrc);
  return r;
}
bool Gates_NAND(JsVar *parent,JsVar *inputArr){
  return !Gates_AND(parent,inputArr);
}
bool Gates_OR(JsVar *parent,JsVar *inputArr){
  bool r = false;bool b;
  JsvIterator itsrc;
  jsvIteratorNew(&itsrc, inputArr, JSIF_EVERY_ARRAY_ELEMENT);
  while (jsvIteratorHasElement(&itsrc)) {
	b = Gates_getPortValue(parent,jsvIteratorGetValue(&itsrc));
	if(b == true) r = true;
    jsvIteratorNext(&itsrc);
  }
  jsvIteratorFree(&itsrc);
  return r;
}
bool Gates_NOR(JsVar *parent,JsVar *inputArr){
  return !Gates_OR(parent,inputArr);
}
bool Gates_EXOR(JsVar *parent,JsVar *inputArr){
  bool r = false; bool tmp;bool b;
  JsvIterator itsrc;
  jsvIteratorNew(&itsrc, inputArr, JSIF_EVERY_ARRAY_ELEMENT);
  tmp = Gates_getPortValue(parent,jsvIteratorGetValue(&itsrc));
  jsvIteratorNext(&itsrc);
  while (jsvIteratorHasElement(&itsrc)) {
    b = Gates_getPortValue(parent,jsvIteratorGetValue(&itsrc));
	if(b != tmp) r = true;
    jsvIteratorNext(&itsrc);
  }
  jsvIteratorFree(&itsrc);
  return r;
}
bool Gates_FLIPFLOP(JsVar *parent,bool out, JsVar *set,JsVar *reset){
  if(Gates_getPortValue(parent,set)) return true;
  else {
    if(Gates_getPortValue(parent,reset)) return false;
	else return out;
  }
}
bool Gates_ROTATE(JsVar *parent,JsVar *args){
  int length,pnt; bool r;
  JsVar *arr = jsvObjectGetChild(args,DigitalGateInputArr,0);
  length = jsvGetInteger(jsvObjectGetChild(args,DigitalGateInputLength,0));
  pnt = jsvGetInteger(jsvObjectGetChild(args,DigitalGateInputPnt,0));
  JsvIterator it; int i = 0;
  jsvIteratorNew(&it, arr, JSIF_EVERY_ARRAY_ELEMENT);
  for(i = 1; i <= pnt; i++) jsvIteratorNext(&it);
  r = Gates_getPortValue(parent,jsvIteratorGetValue(&it));
  pnt++;
  if(pnt >= length) pnt = 0;
  jsvObjectSetChildAndUnLock(args,DigitalGateInputPnt,jsvNewFromInteger(pnt));
  return r;
}
bool Gates_COUNTER(JsVar parent,JsVar *args){
  int max,pnt;
}

bool Gate_Execute(JsVar *parent,int type,bool out, JsVar *args){
  bool r;
    switch(type){
    case 0: r = Gates_AND(parent,jsvObjectGetChild(args,DigitalGateInputArr,0));break;
	case 1: r = Gates_NAND(parent,jsvObjectGetChild(args,DigitalGateInputArr,0));break;
	case 2: r = Gates_OR(parent,jsvObjectGetChild(args,DigitalGateInputArr,0));break;
	case 3: r = Gates_NOR(parent,jsvObjectGetChild(args,DigitalGateInputArr,0));break;
	case 4: r = Gates_EXOR(parent,jsvObjectGetChild(args,DigitalGateInputArr,0));break;
	case 5: r = Gates_FLIPFLOP(parent,out,jsvObjectGetChild(args,DigitalGateInputSet,0),
	                               jsvObjectGetChild(args,DigitalGateInputReset,0));break;
	case 6: r = Gates_ROTATE(parent,args); break;
  }
  return r;
}

//Class Digital
/*JSON{
  "type"	: "class",
  "class"	: "Digital"
}
This is the built-in class for digital functions
*/
/*JSON{
  "type" : "idle",
  "generate" : "jswrap_Digital_idle"
}*/
bool jswrap_Digital_idle() {
  
  return false;
}
/*JSON{
  "type"	: "staticmethod",
  "class"	: "Digital",
  "name"	: "value",
  "generate": "jswrap_Digital_value",
  "params"	: [
    ["port","JsVar","Digital port(Pin, value, function, given port"]
  ],
  "return"	: ["bool","result of given port, mainly for testing"]
}*/
bool jswrap_Digital_value(JsVar *port){
  return Gates_getPortValue(jsvNewFromBool(true),port);
}
/*JSON{
  "type" : "staticproperty",
  "class" : "Digital",
  "name" : "AND",
  "generate_full" : "0",
  "return" : ["int","AND function"]
}*/
/*JSON{
  "type" : "staticproperty",
  "class" : "Digital",
  "name" : "NAND",
  "generate_full" : "1",
  "return" : ["int","NAND function"]
}*/
/*JSON{
  "type" : "staticproperty",
  "class" : "Digital",
  "name" : "OR",
  "generate_full" : "2",
  "return" : ["int","OR function"]
}*/
/*JSON{
  "type" : "staticproperty",
  "class" : "Digital",
  "name" : "NOR",
  "generate_full" : "3",
  "return" : ["int","NOR function"]
}*/
/*JSON{
  "type" : "staticproperty",
  "class" : "Digital",
  "name" : "EXOR",
  "generate_full" : "4",
  "return" : ["int","EXOR function"]
}*/
/*JSON{
  "type" : "staticproperty",
  "class" : "Digital",
  "name" : "FLIPFLOP",
  "generate_full" : "5",
  "return" : ["int","FLIPFLOP function"]
}*/
/*JSON{
  "type" : "staticproperty",
  "class" : "Digital",
  "name" : "ROTATE",
  "generate_full" : "6",
  "return" : ["int","ROTATE function"]
}*/
/*JSON{
  "type" : "staticproperty",
  "class" : "Digital",
  "name" : "COUNTER",
  "generate_full" : "7",
  "return" : ["int","COUNTER function"]
}*/

//Class DigitalGate
void emitOnEdge(JsVar *parent,char *edge){
  JsVar *args[1];
  args[0] = jsvNewFromString(edge);
  JsVar *eventName = jsvNewFromString(JS_EVENT_PREFIX"edge");
  JsVar *callback = jsvSkipNameAndUnLock(jsvFindChildFromVar(parent, eventName, 0));
  jsvUnLock(eventName);
  if (callback) jsiQueueEvents(parent, callback, args, 1);
  jsvUnLock(callback);
  jsvUnLockMany(1, args);
}
void DigitalGate_SetEdge(JsVar *parent,bool old, bool new){
  if(new == old) jsvObjectSetChild(parent,DigitalGateEdge,jsvNewFromString("none"));
  else {
    if(new){
      jsvObjectSetChildAndUnLock(parent,DigitalGateEdge,jsvNewFromString(DigitalGateRaise));
	  emitOnEdge(parent,DigitalGateRaise);
	}
	else{
      jsvObjectSetChildAndUnLock(parent,DigitalGateEdge,jsvNewFromString(DigitalGateFall));
	  emitOnEdge(parent,DigitalGateFall);
	}
  }
}
/*JSON{
  "type"	: "class",
  "class"	: "DigitalGate"
}
This is the built-in class for digital functions
*/
/*JSON{
  "type" : "constructor",
  "class" : "DigitalGate",
  "name" : "DigitalGate",
  "generate" : "jswrap_DigitalGate_constructor",
  "params" : [
    ["type","int","type of digital function (AND,NAND,OR,NOR,EXOR,FLIPFLOP,..."],
    ["args","JsVar","input vars"]
  ],
  "return" : ["JsVar","An Digital function"]
}
Creates an digital function of type, give it a name and store inputs
*/
JsVar *jswrap_DigitalGate_constructor(int type,JsVar *args) {
  int tmp;
  JsVar *parent = jspNewObject(0,DigitalGateID);
  jsvObjectSetChildAndUnLock(parent,DigitalGateType,jsvNewFromInteger(type));
  jsvObjectSetChildAndUnLock(parent,DigitalGateArgs,args);
  jsvObjectSetChildAndUnLock(parent,DigitalGateEdge,jsvNewFromString("none"));
  switch(type){
    case 6: tmp = jsvGetArrayLength(jsvObjectGetChild(args,DigitalGateInputArr,0));
            jsvObjectSetChildAndUnLock(args,DigitalGateInputLength,jsvNewFromInteger(tmp));
			jsvObjectSetChildAndUnLock(args,DigitalGateInputPnt,jsvNewFromInteger(0));
			jsvObjectSetChildAndUnLock(parent,DigitalGateOut,jsvNewFromBool(0));
	        break;
	case 7: jsvObjectSetChildAndUnLock(args,DigitalGateInputCntmax,jsvObjectGetChild(args,DigitalGateInputCntmax));
	        jsvObjectSetChildAndUnLock(args,DigitalGateInputPnt,jsvNewFromInteger(0);
			jsvObjectSetChildAndUnLock(parent,DigitalGateOut,jsvNewFromBool(0));
			break;
	default: jsvObjectSetChildAndUnLock(parent,DigitalGateOut,jsvNewFromBool(Gate_Execute(parent,type,false,args)));
            break;
  }  
  return parent;
}
/*JSON{
  "type" : "event",
  "class" : "DigitalGate",
  "name" : "onEdge"
}
This event is called when status of Digital Gate changes.
*/
/*JSON{
  "type"	: "method",
  "class"	: "DigitalGate",
  "name"	: "execute",
  "generate": "jswrap_DigitalGate_execute",
  "return"	: ["bool","result of executed function"]  
}
Executes digital function
*/
bool jswrap_DigitalGate_execute(JsVar *parent){
  int type = jsvGetInteger(jsvObjectGetChild(parent, DigitalGateType,0));
  JsVar *args = jsvObjectGetChild(parent,DigitalGateArgs,0);
  bool out = jsvGetBool(jsvObjectGetChild(parent,DigitalGateOut,0));
  bool r = Gate_Execute(parent,type, out, args);
  jsvObjectSetChild(parent,DigitalGateOut,jsvNewFromBool(r));
  DigitalGate_SetEdge(parent,out,r);
  return r;
}

