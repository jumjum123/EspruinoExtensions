/*
 * This file is a library for Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2016 Juergen Marsch <juergenmarsch@googlemail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * This file is designed to be parsed during the build process
 *
 * Contains ESP8266 board specific function definitions
 * for handling LedCircle functions (WS2812)
 * 
 * Everything is based on Espruino Source from Neil Kolban
 * and Thorsten van Eicken 
 * ----------------------------------------------------------------------------
 */

#include "jswrap_LedCircle.h"
#include "jsparse.h"
#include "jswrap_arraybuffer.h"
#include "jswrap_espruino.h"
#include "jsinteractive.h"

//jsiConsolePrintf("Show length=%d \n",dataLength);
//jsiConsolePrintStringVar(JsVar *v);
//jsiConsolePrintf_int(const char *fmt, ...);
/*   %d = int
 *   %0#d = int padded to length # with 0s
 *   %x = int as hex
 *   %L = JsVarInt
 *   %Lx = JsVarInt as hex
 *   %f = JsVarFloat
 *   %s = string (char *)
 *   %c = char
 *   %v = JsVar * (doesn't have to be a string - it'll be converted)
 *   %q = JsVar * (in quotes, and escaped)
 *   %j = Variable printed as JSON
 *   %t = Type of variable
 *   %p = Pin
*/
int setPointer(JsLedCircle lm,int circle,int led){
  int p,i;
  p = 0;
  for(i = 0; i < circle; i++){p += lm.data.circleSize[i];}
  p += led;
  return p * 3;
}
static inline void LedCircleStructInit(JsLedCircle *lm){
  lm->data.circleSize[0] = 24;
  lm->data.circleSize[1] = 16;
  lm->data.circleSize[2] = 12;
  lm->data.circleSize[3] = 8;
  lm->data.circleSize[4] = 1;
  lm->data.cells = 61;
  lm->data.cellsLength = 183;
  lm->data.red = 32;
  lm->data.green = 32;
  lm->data.blue = 32; 
}
void LedCircleSetVar(JsLedCircle *lm){
  JsVar *dataname = jsvFindChildFromString(lm->LedCircleVar,JS_HIDDEN_CHAR_STR"lm",true);
  JsVar *data = jsvSkipName(dataname);
  if (!data) {
    data = jsvNewStringOfLength(sizeof(JsLedCircleData));
    jsvSetValueOfName(dataname, data);
  }
  jsvUnLock(dataname);
  assert(data);
  jsvSetString(data, (char*)&lm->data, sizeof(JsLedCircleData));
  jsvUnLock(data);
}
bool LedCircleGetFromVar(JsLedCircle *lm, JsVar *parent){
  lm->LedCircleVar = parent;
  JsVar *data = jsvObjectGetChild(parent,JS_HIDDEN_CHAR_STR"lm",0);
  assert(data);
  if(data){
    jsvGetString(data, (char*)&lm->data, sizeof(JsLedCircleData)+1);
    jsvUnLock(data);
    return true;
  } else
    return false;
}

JsVar *LedCircle_getCircle(JsVar *buf, JsLedCircle lm,int circle){
  JsvArrayBufferIterator its;int p;int i;
  char *ptr = 0;
  p = setPointer(lm,circle,0);
  JsVar *bufa = jsvNewArrayBufferWithPtr(lm.data.circleSize[circle] * 3,&ptr);
  JsvArrayBufferIterator itd;
  jsvArrayBufferIteratorNew(&itd,bufa,0);
  jsvArrayBufferIteratorNew(&its,buf,p);	
  for(int i = 0; i < (lm.data.circleSize[circle] * 3);i++){
    jsvArrayBufferIteratorSetByteValue(&itd,(char)jsvArrayBufferIteratorGetIntegerValue(&its));
    jsvArrayBufferIteratorNext(&itd);
    jsvArrayBufferIteratorNext(&its);
  }
  jsvArrayBufferIteratorFree(&its);
  jsvArrayBufferIteratorFree(&itd);
  return bufa;
}
void LedCircle_setCircle(JsVar *buf,JsLedCircle lm,JsVar *circleBuffer, int circle){
  JsvArrayBufferIterator itd;
  JsvArrayBufferIterator its;
  int p; int i;
  p = setPointer(lm,circle,0);
  jsvArrayBufferIteratorNew(&its,circleBuffer,0);
  jsvArrayBufferIteratorNew(&itd,buf,p);
  for(int i = 0; i < (lm.data.circleSize[circle] * 3); i++){
	jsvArrayBufferIteratorSetByteValue(&itd,(char)jsvArrayBufferIteratorGetIntegerValue(&its));
	jsvArrayBufferIteratorNext(&itd);
	jsvArrayBufferIteratorNext(&its);
  }
  jsvArrayBufferIteratorFree(&itd);
  jsvArrayBufferIteratorFree(&its);
}


/*JSON{
  "type" : "class",
  "class" : "LedCircle"
}
A class to support some simple drawings and animations (rotate and shift)
For LED Cicles like the 5 circle WS2812 board CJMCU
*/
/*JSON{
  "type" : "staticmethod",
  "class" : "LedCircle",
  "name" : "createCircleBuffer",
  "generate" : "jswrap_LedCircle_createCircleBuffer",
  "return" : ["JsVar","new LedCircle object"],
  "return_object" : "LedCircle"
}
Creates LEDCircle object with an Arraybuffer for LED-Circles
This is tested with 5 circle WS2812(CJMCU). 

*/
JsVar *jswrap_LedCircle_createCircleBuffer(){
  JsVar *parent = jspNewObject(0,"LedCircle");
  JsLedCircle lm;
  LedCircleStructInit(&lm);
  lm.LedCircleVar = parent;
  LedCircleSetVar(&lm);
  JsVar *buf = jswrap_arraybuffer_constructor(lm.data.cellsLength);
  jsvUnLock2(jsvAddNamedChild(lm.LedCircleVar,buf,"buffer"),buf);
  return parent;
}

/*JSON{
  "type" : "method",
  "class" : "LedCircle",
  "name" : "setColor",
  "generate" : "jswrap_LedCircle_setColor",
  "params"   : [
    ["red","int","red"],
    ["green","int","green"],
    ["blue","int","blue"]
 ]   
}
Sets color (red,green,blue) for following drawing commands.
Each color is a value from 0 to 255,
Recommendation for stable timing is to use 1 as lowest value
*/
void jswrap_LedCircle_setColor(JsVar *parent, int red, int green, int blue){
  JsLedCircle lm;
  if(!LedCircleGetFromVar(&lm,parent)) return ;
  lm.data.red = red;
  lm.data.green = green;
  lm.data.blue = blue;
  LedCircleSetVar(&lm);
}

/*JSON{
  "type" : "method",
  "class" : "LedCircle",
  "name" : "setColorHSB",
  "generate" : "jswrap_LedCircle_setColorHSB",
  "params"   : [
    ["hue","float","hue"],
    ["sat","float","saturation"],
    ["bri","float","brightness"]
 ]   
}
Sets color for following drawings
Based on hue, saturation and brightness
Values are from 0 to 1
*/
void jswrap_LedCircle_setColorHSB(JsVar *parent, JsVarFloat hue, JsVarFloat sat, JsVarFloat bri){
  JsLedCircle lm;
  if(!LedCircleGetFromVar(&lm,parent)) return ;
  JsVarInt JsVarrgb = jswrap_espruino_HSBtoRGB(hue,sat,bri);
  int rgb = (int)JsVarrgb; lm.data.red = (char)rgb;
  rgb = (rgb >> 8); lm.data.green = (char)rgb;
  rgb = (rgb >> 8); lm.data.blue = (char)rgb;
  LedCircleSetVar(&lm);
}

/*JSON{
  "type" : "method",
  "class" : "LedCircle",
  "name" : "setPixel",
  "generate" : "jswrap_LedCircle_setPixel",
  "params"   : [
    ["circle","int","circle"],
    ["led","int","led"]
 ]   
}
Sets a pixel by circle and led in circle with Color set before
*/
void LedCircle_setPixel(JsVar *buf,JsLedCircle lm,int circle,int led){
  JsvArrayBufferIterator it;int p;int i;
  p = setPointer(lm,circle,led);
  jsvArrayBufferIteratorNew(&it,buf,p);
  jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.green);
  jsvArrayBufferIteratorNext(&it);
  jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.red);
  jsvArrayBufferIteratorNext(&it);
  jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.blue);
  jsvArrayBufferIteratorFree(&it);
}
void jswrap_LedCircle_setPixel(JsVar *parent, int circle, int led ){
  JsLedCircle lm;
  if(!LedCircleGetFromVar(&lm,parent)) return;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  LedCircle_setPixel(buf,lm,circle,led);
  jsvUnLock(buf);
}

/*JSON{
  "type" : "method",
  "class" : "LedCircle",
  "name" : "getPixel",
  "generate" : "jswrap_LedCircle_getPixel",
  "params" : [
    ["circle","int","circle"],
    ["led","int","led"]
  ],
  "return" : ["JsVar","pixelBuffer"] 
}
Returns a buffer with color of pixel defined by circle and led
*/
JsVar *LedCircle_getPixel(JsVar *buf,JsLedCircle lm, int circle,int led){
  JsvArrayBufferIterator it;int p;int i;
  p = setPointer(lm,circle,led);
  jsvArrayBufferIteratorNew(&it,buf,p);
  char *ptr = 0;
  JsVar *bufa = jsvNewArrayBufferWithPtr(3,&ptr);
  JsvArrayBufferIterator itd;int pd;
  jsvArrayBufferIteratorNew(&itd,bufa,0);
  for(int i = 0; i < 3; i++){
    jsvArrayBufferIteratorSetByteValue(&itd,(char)jsvArrayBufferIteratorGetIntegerValue(&it));
    jsvArrayBufferIteratorNext(&it);
    jsvArrayBufferIteratorNext(&itd);
  }
  jsvArrayBufferIteratorFree(&it);
  jsvArrayBufferIteratorFree(&itd);
  return bufa;
}
JsVar *jswrap_LedCircle_getPixel(JsVar *parent, int circle, int led){
  JsLedCircle lm;
  if(!LedCircleGetFromVar(&lm,parent)) return 0;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  JsVar *bufa = LedCircle_getPixel(buf,lm,circle,led);
  jsvUnLock(buf);
  return bufa;	
}

/*JSON{
 "type"     : "method",
 "class"    : "LedCircle",
 "name"     : "fill",
 "generate" : "jswrap_LedCircle_fill"  
}
Fills all circles with same color
*/
void LedCircle_fill(JsVar *buf,JsLedCircle lm){
  JsvArrayBufferIterator it;
  jsvArrayBufferIteratorNew(&it,buf,0);
  for(int i = 0; i < lm.data.cells; i++){
    jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.green);
    jsvArrayBufferIteratorNext(&it);
    jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.red);
    jsvArrayBufferIteratorNext(&it);
    jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.blue);
    jsvArrayBufferIteratorNext(&it);
  }
  jsvArrayBufferIteratorFree(&it);
}
void jswrap_LedCircle_fill(JsVar *parent){
  JsLedCircle lm;
  if(!LedCircleGetFromVar(&lm,parent)) return ;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  LedCircle_fill(buf,lm);
  jsvUnLock(buf);
}

/*JSON{
 "type"     : "method",
 "class"    : "LedCircle",
 "name"     : "fillCircle",
 "generate" : "jswrap_LedCircle_fillCircle",
 "params"   : [
    ["circle","int","circle"]
 ]  
}
Draws a full circle
*/
void LedCircle_fillCircle(JsVar *buf, JsLedCircle lm, int circle){
  JsvArrayBufferIterator it;int p;int i;
  p = setPointer(lm,circle,0);
  for(int i = 0; i < lm.data.circleSize[circle]; i++){
    jsvArrayBufferIteratorNew(&it,buf,p);
    jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.green);
    jsvArrayBufferIteratorNext(&it);
    jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.red);
    jsvArrayBufferIteratorNext(&it);
    jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.blue);
    jsvArrayBufferIteratorNext(&it);
    jsvArrayBufferIteratorFree(&it);
    p += 3;
  }
}
void jswrap_LedCircle_fillCircle(JsVar *parent, int circle){
  JsLedCircle lm;
  if(!LedCircleGetFromVar(&lm,parent)) return ;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  LedCircle_fillCircle(buf, lm, circle);
  jsvUnLock(buf);
}

/*JSON{
  "type" : "method",
  "class" : "LedCircle",
  "name" : "getCircle",
  "generate" : "jswrap_LedCircle_getCircle",
  "params" : [["circle","int","circle"]],
  "return" : ["JsVar","circleBuffer"]
}
Get a buffer with pixeldata from one led
*/
JsVar *jswrap_LedCircle_getCircle(JsVar *parent,int circle){
  JsLedCircle lm;
  if(!LedCircleGetFromVar(&lm,parent)) return 0;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  JsVar *bufa = LedCircle_getCircle(buf,lm,circle);
  jsvUnLock(buf);
  return bufa;
}

/*JSON{
  "type" : "method",
  "class" : "LedCircle",
  "name" : "setCircle",
  "generate" : "jswrap_LedCircle_setCircle",
  "params" : [
    ["circleBuffer","JsVar","circleBuffer"],
    ["circle","int","circle"]
  ]
}
Sets a circle in internal buffer from buffer in circle
*/
void jswrap_LedCircle_setCircle(JsVar *parent, JsVar *circleBuffer, int circle){
  JsLedCircle lm;
  if(!LedCircleGetFromVar(&lm,parent)) return;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  LedCircle_setCircle(buf, lm, circleBuffer,circle);
  jsvUnLock(buf);
}

/*JSON{
  "type" : "method",
  "class" : "LedCircle",
  "name" : "rotate",
  "generate" : "jswrap_LedCircle_rotate",
  "params" : [
    ["direction","int","direction"],
	["circle","int","circle"]
  ]
}
Rotates pixel in circle, 2 = left, 3 = right 
*/
void LedCircle_rotate(JsVar *circleBuffer,JsLedCircle lm,int direction, int circle){
  JsvArrayBufferIterator its,itd; int p; int i;
  JsVar *bufa = LedCircle_getCircle(circleBuffer,lm,circle);
  switch(direction){
	case 2:
      p = setPointer(lm,circle,0);
      jsvArrayBufferIteratorNew(&itd,circleBuffer,p);
	  jsvArrayBufferIteratorNew(&its,bufa,3);
	  for(i = 3; i < (lm.data.circleSize[circle] * 3); i++){
		jsvArrayBufferIteratorSetByteValue(&itd,(char)jsvArrayBufferIteratorGetIntegerValue(&its));
	    jsvArrayBufferIteratorNext(&itd);
	    jsvArrayBufferIteratorNext(&its);
	  }
	  jsvArrayBufferIteratorFree(&its);
	  jsvArrayBufferIteratorNew(&its,bufa,0);
	  for(i = 0; i < 3; i++){
		jsvArrayBufferIteratorSetByteValue(&itd,(char)jsvArrayBufferIteratorGetIntegerValue(&its));
	    jsvArrayBufferIteratorNext(&itd);
	    jsvArrayBufferIteratorNext(&its);		
	  }
	  jsvArrayBufferIteratorFree(&its);
	  jsvArrayBufferIteratorFree(&itd);
	  break;
	case 3:
	  p = setPointer(lm,circle,1);
	  jsvArrayBufferIteratorNew(&itd,circleBuffer,p);
	  jsvArrayBufferIteratorNew(&its,bufa,0);
	  for(i = 3; i < (lm.data.circleSize[circle] * 3); i++){
		jsvArrayBufferIteratorSetByteValue(&itd,(char)jsvArrayBufferIteratorGetIntegerValue(&its));
	    jsvArrayBufferIteratorNext(&itd);
	    jsvArrayBufferIteratorNext(&its);
	  }
	  jsvArrayBufferIteratorFree(&itd);
	  p = setPointer(lm,circle,0);
	  jsvArrayBufferIteratorNew(&itd,circleBuffer,p);
	  for(i = 0; i < 3; i++){
		jsvArrayBufferIteratorSetByteValue(&itd,(char)jsvArrayBufferIteratorGetIntegerValue(&its));
	    jsvArrayBufferIteratorNext(&itd);
	    jsvArrayBufferIteratorNext(&its);		
	  }
	  jsvArrayBufferIteratorFree(&its);
	  jsvArrayBufferIteratorFree(&itd);      
	  break;
  }
  jsvUnLock(bufa);
}
void jswrap_LedCircle_rotate(JsVar *parent, int direction, int circle){
  JsLedCircle lm;
  if(!LedCircleGetFromVar(&lm,parent)) return;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  LedCircle_rotate(buf, lm, direction, circle);
  jsvUnLock(buf);
}
