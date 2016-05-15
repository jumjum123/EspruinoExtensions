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

#include "jswrap_LedMatrix.h"
#include "libneoPixel.h"
#include "jsparse.h"
#include "jswrap_arraybuffer.h"
#include "jswrap_espruino.h"
#include "jsinteractive.h"

//jsiConsolePrintf("Show length=%d \n",dataLength);

static inline void LedMatrixStructInit(JsLedMatrix *lm){
  lm->data.width = 8;
  lm->data.height = 8;
  lm->data.cells = 64;
  lm->data.cellsLength = 192;
  lm->data.red = 32;
  lm->data.green = 32;
  lm->data.blue = 32;
  lm->data.oneBitDuration = 56;
  lm->data.zeroBitDuration = 14;
}

void LedMatrixSetVar(JsLedMatrix *lm){
  JsVar *dataname = jsvFindChildFromString(lm->LedMatrixVar,JS_HIDDEN_CHAR_STR"lm",true);
  JsVar *data = jsvSkipName(dataname);
  if (!data) {
    data = jsvNewStringOfLength(sizeof(JsLedMatrixData));
    jsvSetValueOfName(dataname, data);
  }
  jsvUnLock(dataname);
  assert(data);
  jsvSetString(data, (char*)&lm->data, sizeof(JsLedMatrixData));
  jsvUnLock(data);
}

bool LedMatrixGetFromVar(JsLedMatrix *lm, JsVar *parent){
  lm->LedMatrixVar = parent;
  JsVar *data = jsvObjectGetChild(parent,JS_HIDDEN_CHAR_STR"lm",0);
  assert(data);
  if(data){
    jsvGetString(data, (char*)&lm->data, sizeof(JsLedMatrixData)+1);
    jsvUnLock(data);
    return true;
  } else
    return false;
}

/*JSON{
  "type" : "class",
  "class" : "LedMatrix"
}
*/
/*JSON{
  "type" : "staticmethod",
  "class" : "LedMatrix",
  "name" : "createArrayBuffer",
  "generate" : "jswrap_LedMatrix_createArrayBuffer",
  "params" :[
    ["width","int32","width"],
    ["height","int32","height"],
    ["pin","pin","pin"]
  ],
  "return" : ["JsVar","new LedMatrix object"],
  "return_object" : "LedMatrix"
}
*/
JsVar *jswrap_LedMatrix_createArrayBuffer(int width,int height,Pin pin){
  JsVar *parent = jspNewObject(0,"LedMatrix");
  JsLedMatrix lm;
  LedMatrixStructInit(&lm);
  lm.LedMatrixVar = parent;
  lm.data.width = width;
  lm.data.height = height;
  lm.data.cells = width * height;
  lm.data.cellsLength = width * height * 3;
  lm.data.pin = pin;
  LedMatrixSetVar(&lm);
  JsVar *buf = jswrap_arraybuffer_constructor(lm.data.cellsLength);
  jsvUnLock2(jsvAddNamedChild(lm.LedMatrixVar,buf,"buffer"),buf);
  return parent;
}

/*JSON{
  "type" : "method",
  "class" : "LedMatrix",
  "name" : "show",
  "generate" : "jswrap_LedMatrix_show"
}*/
void jswrap_LedMatrix_show(JsVar *parent){
  JsLedMatrix lm;
  if(!LedMatrixGetFromVar(&lm,parent)) return ;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  JSV_GET_AS_CHAR_ARRAY(pixels, dataLength, buf);
  uint8_t *p = (uint8_t *)pixels;
  uint8_t *end = p + lm.data.cellsLength;
  sendToNeopixel(lm.data.pin,p,end,lm.data.zeroBitDuration,lm.data.oneBitDuration);
  jsvUnLock(buf);
}

/*JSON{
  "type":"method",
  "class" : "LedMatrix",
  "name" : "setPulseLength",
  "generate" : "jswrap_LedMatrix_setPulseLength",
  "params"   : [
    ["zeroBitDuration","int","zeroLength"],
    ["oneBitDuration","int","oneLength"]
 ]   
}*/
void jswrap_LedMatrix_setPulseLength(JsVar *parent, int zeroBitDuration, int oneBitDuration){
  JsLedMatrix lm;
  if(!LedMatrixGetFromVar(&lm,parent)) return ;
  lm.data.oneBitDuration = oneBitDuration;
  lm.data.zeroBitDuration = zeroBitDuration;
  LedMatrixSetVar(&lm);
}

/*JSON{
  "type" : "method",
  "class" : "LedMatrix",
  "name" : "setColor",
  "generate" : "jswrap_LedMatrix_setColor",
  "params"   : [
    ["red","int","red"],
    ["green","int","green"],
    ["blue","int","blue"]
 ]   
}*/
void jswrap_LedMatrix_setColor(JsVar *parent, int red, int green, int blue){
  JsLedMatrix lm;
  if(!LedMatrixGetFromVar(&lm,parent)) return ;
  lm.data.red = red;
  lm.data.green = green;
  lm.data.blue = blue;
  LedMatrixSetVar(&lm);
}

/*JSON{
  "type" : "method",
  "class" : "LedMatrix",
  "name" : "setColorHSB",
  "generate" : "jswrap_LedMatrix_setColorHSB",
  "params"   : [
    ["hue","float","hue"],
    ["sat","float","saturation"],
    ["bri","float","brightness"]
 ]   
}*/
void jswrap_LedMatrix_setColorHSB(JsVar *parent, JsVarFloat hue, JsVarFloat sat, JsVarFloat bri){
  JsLedMatrix lm;
  if(!LedMatrixGetFromVar(&lm,parent)) return ;
  JsVarInt JsVarrgb = jswrap_espruino_HSBtoRGB(hue,sat,bri);
  int rgb = (int)JsVarrgb; lm.data.red = (char)rgb;
  rgb = (rgb >> 8); lm.data.green = (char)rgb;
  rgb = (rgb >> 8); lm.data.blue = (char)rgb;
  LedMatrixSetVar(&lm);
}

/*JSON{
  "type" : "method",
  "class" : "LedMatrix",
  "name" : "setPixel",
  "generate" : "jswrap_LedMatrix_setPixel",
  "params"   : [
    ["row","int","row"],
    ["column","int","column"]
 ]   
}*/
void LedMatrix_setPixel(JsVar *buf,JsLedMatrix lm,int row,int column){
  JsvArrayBufferIterator it;int p;
  p = (row * lm.data.width + column) * 3;
  jsvArrayBufferIteratorNew(&it,buf,p);
  jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.green);
  jsvArrayBufferIteratorNext(&it);
  jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.red);
  jsvArrayBufferIteratorNext(&it);
  jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.blue);
  jsvArrayBufferIteratorFree(&it);
}
void jswrap_LedMatrix_setPixel(JsVar *parent, int row, int column ){
  JsLedMatrix lm;
  if(!LedMatrixGetFromVar(&lm,parent)) return;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  LedMatrix_setPixel(buf,lm,row,column);
  jsvUnLock(buf);
}

/*JSON{
  "type" : "method",
  "class" : "LedMatrix",
  "name" : "getPixel",
  "generate" : "jswrap_LedMatrix_getPixel",
  "params" : [
    ["row","int","row"],
    ["column","int","column"]
  ],
  "return" : ["JsVar","pixelBuffer"] 
}*/
JsVar *LedMatrix_getPixel(JsVar *buf,JsLedMatrix lm, int row,int column){
  JsvArrayBufferIterator it;int p;
  p = (row * lm.data.width + column) *3;
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
JsVar *jswrap_LedMatrix_getPixel(JsVar *parent, int row, int column){
  JsLedMatrix lm;
  if(!LedMatrixGetFromVar(&lm,parent)) return 0;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  JsVar *bufa = LedMatrix_getPixel(buf,lm,row,column);
  jsvUnLock(buf);
  return bufa;	
}

/*JSON{
 "type"     : "method",
 "class"    : "LedMatrix",
 "name"     : "fill",
 "generate" : "jswrap_LedMatrix_fill"  
}*/
void LedMatrix_fill(JsVar *buf,JsLedMatrix lm){
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
void jswrap_LedMatrix_fill(JsVar *parent){
  JsLedMatrix lm;
  if(!LedMatrixGetFromVar(&lm,parent)) return ;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  LedMatrix_fill(buf,lm);
  jsvUnLock(buf);
}

/*JSON{
 "type"     : "method",
 "class"    : "LedMatrix",
 "name"     : "drawColumn",
 "generate" : "jswrap_LedMatrix_drawColumn",
 "params"   : [
    ["column","int","column"]
 ]  
}*/
void LedMatrix_drawColumn(JsVar *buf, JsLedMatrix lm, int column){
  JsvArrayBufferIterator it;int p;
  p = column * 3;
  for(int i = 0; i < lm.data.height; i++){
    jsvArrayBufferIteratorNew(&it,buf,p);
    jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.green);
    jsvArrayBufferIteratorNext(&it);
    jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.red);
    jsvArrayBufferIteratorNext(&it);
    jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.blue);
    jsvArrayBufferIteratorNext(&it);
    jsvArrayBufferIteratorFree(&it);
    p += lm.data.width * 3;
  }
}
void jswrap_LedMatrix_drawColumn(JsVar *parent, int column){
  JsLedMatrix lm;
  if(!LedMatrixGetFromVar(&lm,parent)) return ;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  LedMatrix_drawColumn(buf, lm, column);
  jsvUnLock(buf);
}

/*JSON{
 "type"     : "method",
 "class"    : "LedMatrix",
 "name"     : "drawRow",
 "generate" : "jswrap_LedMatrix_drawRow",
 "params"   : [
    ["row","int","row"]
 ]  
}*/
void LedMatrix_drawRow(JsVar *buf, JsLedMatrix lm, int row){
  JsvArrayBufferIterator it;int p;
  p = row * lm.data.width * 3;
  jsvArrayBufferIteratorNew(&it,buf,p);
  for(int i = 0; i < lm.data.width; i++){
    jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.green);
    jsvArrayBufferIteratorNext(&it);
    jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.red);
    jsvArrayBufferIteratorNext(&it);
    jsvArrayBufferIteratorSetByteValue(&it,(char)lm.data.blue);
    jsvArrayBufferIteratorNext(&it);
  }
  jsvArrayBufferIteratorFree(&it);
}
void jswrap_LedMatrix_drawRow(JsVar *parent, int row){
  JsLedMatrix lm;
  if(!LedMatrixGetFromVar(&lm,parent)) return ;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  LedMatrix_drawRow(buf, lm, row);
  jsvUnLock(buf);
}

/*JSON{
  "type" : "method",
  "class" : "LedMatrix",
  "name" : "getRow",
  "generate" : "jswrap_LedMatrix_getRow",
  "params" : [["row","int","row"]],
  "return" : ["JsVar","rowBuffer"]
}*/
JsVar *LedMatrix_getRow(JsVar *buf, JsLedMatrix lm, int row){
  JsvArrayBufferIterator it;int p;
  p = row * lm.data.width * 3;
  jsvArrayBufferIteratorNew(&it,buf,p);
  char *ptr = 0;
  JsVar *bufa = jsvNewArrayBufferWithPtr(lm.data.width * 3,&ptr);
  JsvArrayBufferIterator itd;int pd;
  jsvArrayBufferIteratorNew(&itd,bufa,0);
  for(int i = 0; i < lm.data.width * 3; i++){
    jsvArrayBufferIteratorSetByteValue(&itd,(char)jsvArrayBufferIteratorGetIntegerValue(&it));
    jsvArrayBufferIteratorNext(&it);
    jsvArrayBufferIteratorNext(&itd);
  }
  jsvArrayBufferIteratorFree(&it);
  jsvArrayBufferIteratorFree(&itd);
  return bufa;  
}
JsVar *jswrap_LedMatrix_getRow(JsVar *parent,int row){
  JsLedMatrix lm;
  if(!LedMatrixGetFromVar(&lm,parent)) return 0;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  JsVar *bufa = LedMatrix_getRow(buf,lm,row);
  jsvUnLock(buf);
  return bufa;
}

/*JSON{
  "type" : "method",
  "class" : "LedMatrix",
  "name" : "setRow",
  "generate" : "jswrap_LedMatrix_setRow",
  "params" : [
    ["rowBuffer","JsVar","rowBuffer"],
    ["row","int","row"]
  ]
}*/
void LedMatrix_setRow(JsVar *buf,JsLedMatrix lm, JsVar *rowBuffer,int row){
  JsvArrayBufferIterator itd;int pd;
  int p = row * lm.data.width * 3;
  jsvArrayBufferIteratorNew(&itd,buf,p);
  JsvArrayBufferIterator its;int ps;
  jsvArrayBufferIteratorNew(&its,rowBuffer,0);
  for(int i = 0; i < lm.data.width *3;i++){
    jsvArrayBufferIteratorSetByteValue(&itd,(char)jsvArrayBufferIteratorGetIntegerValue(&its));
    jsvArrayBufferIteratorNext(&itd);
    jsvArrayBufferIteratorNext(&its);
  }
  jsvArrayBufferIteratorFree(&itd);
  jsvArrayBufferIteratorFree(&its);
}
void jswrap_LedMatrix_setRow(JsVar *parent, JsVar *rowBuffer, int row){
  JsLedMatrix lm;
  if(!LedMatrixGetFromVar(&lm,parent)) return;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  LedMatrix_setRow(buf, lm, rowBuffer,row);
  jsvUnLock(buf);
}

/*JSON{
  "type" : "method",
  "class" : "LedMatrix",
  "name" : "getColumn",
  "generate" : "jswrap_LedMatrix_getColumn",
  "params" : [["column","int","column"]],
  "return" : ["JsVar","columnBuffer"]
}*/
JsVar *LedMatrix_getColumn(JsVar *buf, JsLedMatrix lm, int column){
  JsvArrayBufferIterator its;int p;
  char *ptr = 0;
  JsVar *bufa = jsvNewArrayBufferWithPtr(lm.data.height * 3,&ptr);
  JsvArrayBufferIterator itd;
  jsvArrayBufferIteratorNew(&itd,bufa,0);
  p = column * 3;
  for(int i = 0; i < lm.data.height; i++){
    jsvArrayBufferIteratorNew(&its,buf,p);	
	for(int j = 0; j < 3;j++){
      jsvArrayBufferIteratorSetByteValue(&itd,(char)jsvArrayBufferIteratorGetIntegerValue(&its));
      jsvArrayBufferIteratorNext(&itd);
      jsvArrayBufferIteratorNext(&its);
	}
	jsvArrayBufferIteratorFree(&its);
    p += lm.data.width * 3;
  }
  jsvArrayBufferIteratorFree(&itd);
  return bufa;
}
JsVar *jswrap_LedMatrix_getColumn(JsVar *parent,int column){
  JsLedMatrix lm;
  if(!LedMatrixGetFromVar(&lm,parent)) return 0;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  JsVar *bufa = LedMatrix_getColumn(buf,lm,column);
  jsvUnLock(buf);
  return bufa;
}

/*JSON{
  "type" : "method",
  "class" : "LedMatrix",
  "name" : "setColumn",
  "generate" : "jswrap_LedMatrix_setColumn",
  "params" : [
    ["rowBuffer","JsVar","columnBuffer"],
    ["row","int","column"]
  ]
}*/
void LedMatrix_setColumn(JsVar *buf,JsLedMatrix lm, JsVar *columnBuffer,int column){
  JsvArrayBufferIterator itd;int pd;
  int p = column * 3;
  JsvArrayBufferIterator its;int ps;
  jsvArrayBufferIteratorNew(&its,columnBuffer,0);
  for(int i = 0; i < lm.data.height; i++){
    jsvArrayBufferIteratorNew(&itd,buf,p);
    for(int j = 0; j < 3; j++){
	  jsvArrayBufferIteratorSetByteValue(&itd,(char)jsvArrayBufferIteratorGetIntegerValue(&its));
	  jsvArrayBufferIteratorNext(&itd);
	  jsvArrayBufferIteratorNext(&its);
	}
	jsvArrayBufferIteratorFree(&itd);
	p += lm.data.width *3;  
  }
  jsvArrayBufferIteratorFree(&its);
}
void jswrap_LedMatrix_setColumn(JsVar *parent, JsVar *columnBuffer, int column){
  JsLedMatrix lm;
  if(!LedMatrixGetFromVar(&lm,parent)) return;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  LedMatrix_setColumn(buf, lm, columnBuffer,column);
  jsvUnLock(buf);
}

/*JSON{
  "type" : "method",
  "class" : "LedMatrix",
  "name" : "shift",
  "generate" : "jswrap_LedMatrix_shift",
  "params" : [
    ["direction","int","direction"]
  ]
}*/
void LedMatrix_shift(JsVar *buf, JsLedMatrix lm, int direction){
    JsVar *tmp;
    switch(direction) {
    case 0: //up
      for(int i = 1; i < lm.data.height; i++){
		tmp = LedMatrix_getRow(buf,lm,i);
		LedMatrix_setRow(buf,lm,tmp,(i-1));
		jsvUnLock(tmp);  
	  }
      break;
    case 1: //down
      for(int i = lm.data.height - 1; i > 0; i--){
		tmp = LedMatrix_getRow(buf,lm,(i - 1));
		LedMatrix_setRow(buf,lm,tmp,i);
		jsvUnLock(tmp);
	  } 
      break;
    case 2: //left
      for(int i = 1; i < lm.data.width;i++){
		tmp = LedMatrix_getColumn(buf,lm,(i));
		LedMatrix_setColumn(buf,lm,tmp,i - 1);
		jsvUnLock(tmp);
	  }
      break;
    case 3: //right
      for(int i = lm.data.width - 1; i > 0; i--){
		tmp = LedMatrix_getColumn(buf,lm,(i - 1));
		LedMatrix_setColumn(buf,lm,tmp,i);
		jsvUnLock(tmp);  
	  }
      break;
  }
}
void jswrap_LedMatrix_shift(JsVar *parent, int direction){
  JsLedMatrix lm;
  if(!LedMatrixGetFromVar(&lm,parent)) return;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  LedMatrix_shift(buf,lm,direction);
  jsvUnLock(buf);
}

/*JSON{
  "type" : "method",
  "class" : "LedMatrix",
  "name" : "rotate",
  "generate" : "jswrap_LedMatrix_rotate",
  "params" : [
    ["direction","int","direction"]
  ]
}*/
void LedMatrix_rotate(JsVar *buf, JsLedMatrix lm, int direction){
    JsVar *tmp;
    switch(direction) {
    case 0: //up
      tmp = LedMatrix_getRow(buf,lm,0);
      LedMatrix_shift(buf,lm,direction);
      LedMatrix_setRow(buf,lm,tmp,lm.data.height-1);
      jsvUnLock(tmp);
      break;
    case 1: //down
      tmp = LedMatrix_getRow(buf,lm,lm.data.height - 1);
      LedMatrix_shift(buf,lm,direction);
      LedMatrix_setRow(buf,lm,tmp,0);
      jsvUnLock(tmp);
      break;
    case 2: //left
      tmp = LedMatrix_getColumn(buf,lm,0);
      LedMatrix_shift(buf,lm,direction);
      LedMatrix_setColumn(buf,lm,tmp,lm.data.width - 1);
      jsvUnLock(tmp);
      break;
    case 3: //right
      tmp = LedMatrix_getColumn(buf,lm,lm.data.width - 1);
      LedMatrix_shift(buf,lm,direction);
      LedMatrix_setColumn(buf,lm,tmp,0);
      jsvUnLock(tmp);
      break;
  }
}
void jswrap_LedMatrix_rotate(JsVar *parent, int direction){
  JsLedMatrix lm;
  if(!LedMatrixGetFromVar(&lm,parent)) return;
  JsVar *buf = jsvObjectGetChild(parent,"buffer",false);
  LedMatrix_rotate(buf,lm,direction);
  jsvUnLock(buf);
}

