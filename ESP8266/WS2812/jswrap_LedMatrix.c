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
 * for handling LedMatrix functions (WS2812)
 * 
 * Everything is based on Espruino Source from Neil Kolban
 * and Thorsten van Eicken 
 * ----------------------------------------------------------------------------
 */

#include "jswrap_LedMatrix.h"
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
A class to support some simple drawings and animations (rotate and shift)
For a LED Matrix like the 8*8 WS2812 board
*/
/*JSON{
  "type" : "staticmethod",
  "class" : "LedMatrix",
  "name" : "createArrayBuffer",
  "generate" : "jswrap_LedMatrix_createArrayBuffer",
  "params" :[
    ["width","int32","width"],
    ["height","int32","height"]
  ],
  "return" : ["JsVar","new LedMatrix object"],
  "return_object" : "LedMatrix"
}
Creates LEDMatrix object with an Arraybuffer for a LED-Matrix
This is tested with 8*8 WS2812 matrix. Others should work.
For other types with different timing, see setPulseLength

*/
JsVar *jswrap_LedMatrix_createArrayBuffer(int width,int height){
  JsVar *parent = jspNewObject(0,"LedMatrix");
  JsLedMatrix lm;
  LedMatrixStructInit(&lm);
  lm.LedMatrixVar = parent;
  lm.data.width = width;
  lm.data.height = height;
  lm.data.cells = width * height;
  lm.data.cellsLength = width * height * 3;
  LedMatrixSetVar(&lm);
  JsVar *buf = jswrap_arraybuffer_constructor(lm.data.cellsLength);
  jsvUnLock2(jsvAddNamedChild(lm.LedMatrixVar,buf,"buffer"),buf);
  return parent;
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
}
Sets color (red,green,blue) for following drawing commands.
Each color is a value from 0 to 255,
Recommendation for stable timing is to use 1 as lowest value
*/
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
}
Sets color for following drawings
Based on hue, saturation and brightness
Values are from 0 to 1
*/
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
}
Sets a pixel by row andcolumn with Color set before
*/
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
}
Returns a buffer with color of pixel defined by row and column
*/
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
}
Fills whole matrix with same color
*/
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
}
Draws a full column
*/
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
}
draws a full row
*/
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
}
Returns an arraybuffer with pixel data from one row
*/
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
}
Sets a row in internal buffer from buffer in row
*/
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
}
Get a buffer with pixeldata from one column
*/
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
}
Sets a column in internal buffer from buffer in column
*/
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
}
Shifts whole matrix into direction and clears new pixels
0 = up, 1 = down, 2 = left, 3 = right
*/
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
}
Rotates pixel data in matrix
0 = up, 1 = down, 2 = left, 3 = right
*/
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


