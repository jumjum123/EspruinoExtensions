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
 
#include "jswrap_LedBoard.h"
#include "jsvariterator.h"
#include "jswrap_espruino.h"
#include "jswrap_arraybuffer.h"
#include "jsinteractive.h"
#include "graphics.h"

bool eventStatus = false;
typedef struct {int x1; int y1; int x2; int y2; int color;} frameLocation;
frameLocation frameLoc;
uint8_t *dataPnt;

/*JSON{
  "type"  : "class",
  "class" : "LedBoard"
}
This is the built-in class for LED Boards like 64x32 (P2.5,P3,P4,...)
*/
/*JSON{
  "type" : "event",
  "class" : "LedBoard",
  "name" : "onpostRefresh"
}
This event is called when sending RGB data to board finished
*/
/*JSON{
  "type" : "staticmethod",
  "class" : "LedBoard",
  "name" : "init",
  "generate" : "jswrap_LedBoard_init",
  "params"   : [
    ["dataPins","JsVar","An array of pins for RGB data [R1,R2,G1,G2,B1,B2"],
	["addressPins","JsVar","An array of pins for adress [A,B,C,D,..]"],
	["controlPins","JsVar","array with [Enable,Latch,Clock]"]
  ]
}
sets all pins for LED Board
*/
void initPins(JsVar *pins,int type){
  Pin pin; int i; JsvObjectIterator it;
  if(jsvIsArray(pins)){
	jsvObjectIteratorNew(&it,pins);
	i = 0;
	while(jsvObjectIteratorHasValue(&it)){
      pin = jshGetPinFromVarAndUnLock(jsvObjectIteratorGetValue(&it));
	  jshPinSetState(pin,JSHPINSTATE_GPIO_OUT);
	  switch(type){
		case 0: if(!LedBoardSaveDataPin(pin,i)) jsError("dataPin not valid"); break;
		case 1: if(!LedBoardSaveAddressPin(pin,i)) jsError("addressPin not valid"); break;
		case 2: if(!LedBoardSaveControlPin(pin,i)) jsError("controlPin not valid"); break;
	  }
	  i++;
	  jsvObjectIteratorNext(&it);
	}
    if(type == 1){
      addressPinsLength = i;
      pixelRows = 1<<i;
    }
	jsvObjectIteratorFree(&it);
  }
}
void jswrap_LedBoard_init(JsVar *dataPins,JsVar *addressPins,JsVar *controlPins){
  initPins(dataPins,0);
  initPins(addressPins,1);
  initPins(controlPins,2);
  LedBoardSetDefault();
}
/*JSON{
  "type"     : "staticmethod",
  "class"    : "LedBoard",
  "name"     : "send",
  "generate" : "jswrap_LedBoard_send",
  "params"   : [
    ["graphics", "JsVar", "Graphics Object"]
  ]
}
  send graphics buffer to Led board
*/
void jswrap_LedBoard_send(JsVar *graphics){
  int i,rgbdata;uint8_t *dataPnt;
  JsVar *buffer = jsvObjectGetChild(graphics,"buffer",0);
  pixelsInRow = jsvGetLength(buffer) / pixelRows;
  dataPnt = jswrap_espruino_getAddressOf(buffer,true);
  for(i = 0; i < pixelRows; i++){
    jshInterruptOff();
	LedBoardSetEnable(false);
	LedBoardSendRow(dataPnt);
	LedBoardSetEnable(true);
    LedBoardSetAddress(i);
	LedBoardPulseLatch();
	LedBoardSetEnable(false);
	dataPnt += pixelsInRow;
    jshInterruptOn();
  }
  LedBoardSetEnable(true);
  if(eventStatus){
	jsiExecuteEventCallbackOn("LedBoard", POSTREFRESH_CALLBACK_NAME, 0, 0);
    eventStatus = false;
  }
}
/*JSON{
  "type"     : "staticmethod",
  "class"    : "LedBoard",
  "name"     : "activateEvent",
  "generate" : "jswrap_LedBoard_activateEvent",
  "params"   : [
    ["status", "bool", "Event status"]
  ]
}
  Activate event postRefresh, or deactivate	
}*/
void jswrap_LedBoard_activateEvent(bool status){
  eventStatus = status;
}
/*JSON{
  "type"     : "staticmethod",
  "class"    : "LedBoard",
  "name"     : "rotate",
  "generate" : "jswrap_LedBoard_rotate",
  "params"   : [
    ["graphics", "JsVar", "graphics Object"],
	["direction", "int", "1=left, 2 = right"],
	["frame", "JsVar", "optional frame (x1,y1,x2,y2,color) "]
  ]
}
  rotate graphics buffer Led board
*/
int get_Color(int x,int y){
  uint8_t * pnt; int val;
  pnt = dataPnt + x + (y & (pixelRows - 1)) * pixelsInRow;
  val = *pnt;
  if(y >= pixelRows) (val = val>>4);
  return val & 7;
}
int set_Color(int x,int y, int color){
  uint8_t *pnt;int val;
  pnt = dataPnt + x + (y & (pixelRows - 1)) * pixelsInRow;
  val = *pnt;
  if(y >= pixelRows) 
    val = (val & 0x0F) + (color<<4); 
    else val = (val & 0xF0) + color;
  *pnt = val;
}
int getChildDefault(JsVar *parent, const char *name, int deflt){
  int val;
  JsVar *tmp = jsvObjectGetChild(parent,name,0);
  if(tmp) val = jsvGetInteger(tmp);
  else val = deflt;
  jsvUnLock(tmp);
  return val;
}
void setFrameLoc(JsVar *frame){
  if(!frame){
    frameLoc.x1 = 0;
	frameLoc.y1 = 0;
	frameLoc.x2 = pixelsInRow - 1;
	frameLoc.y2 = pixelRows + pixelRows - 1;
	frameLoc.color = -1;
  }
  else {
    frameLoc.x1 = getChildDefault(frame,"x1",0);
	frameLoc.y1 = getChildDefault(frame,"y1",0);
	frameLoc.x2 = getChildDefault(frame,"x2",pixelsInRow - 1);
	frameLoc.y2 = getChildDefault(frame,"y2",pixelRows + pixelRows - 1);
	frameLoc.color = getChildDefault(frame,"color",-1);
  }
}
void rotate_left(){
  int tmp,x,y;
  for(y = frameLoc.y1; y <= frameLoc.y2; y++){
    if(frameLoc.color < 0) tmp = get_Color(frameLoc.x1,y);
	else tmp = frameLoc.color;
	for(x = frameLoc.x1; x < frameLoc.x2; x++){
      set_Color(x,y,get_Color(x + 1,y));
    }
	set_Color(frameLoc.x2,y,tmp);
  }
}
void rotate_right(){
  int x,y,tmp;
  for(y = frameLoc.y1; y <= frameLoc.y2; y++){
    if(frameLoc.color < 0) tmp = get_Color(frameLoc.x2,y);
	else tmp = frameLoc.color;
	for(x = frameLoc.x2; x > frameLoc.x1; x--){
      set_Color(x,y,get_Color(x - 1,y));
    }
	set_Color(frameLoc.x1,y,tmp);
  }
}
void rotate_up(){
  int x,y,tmp;
  for(x = frameLoc.x1; x <= frameLoc.x2; x++){
    if(frameLoc.color < 0) tmp = get_Color(x,frameLoc.y1);
	else tmp = frameLoc.color;
    for(y = frameLoc.y1; y < frameLoc.y2; y++){
      set_Color(x,y,get_Color(x,y + 1));
	}
	set_Color(x,frameLoc.y2,tmp);
  }
}
void rotate_down(){
  int x,y,tmp;
  for(x = frameLoc.x1; x <= frameLoc.x2; x++){
    if(frameLoc.color < 0) tmp = get_Color(x,frameLoc.y2);
	else tmp = frameLoc.color;
    for(y = frameLoc.y2; y > frameLoc.y1; y--){
      set_Color(x,y,get_Color(x,y - 1));
	}
	set_Color(x,frameLoc.y1,tmp);
  }
}
void jswrap_LedBoard_rotate(JsVar *graphics, int direction,JsVar *frame){
  int tmpPixel,i,j;
  setFrameLoc(frame);
  JsVar *buffer = jsvObjectGetChild(graphics,"buffer",0);
  pixelsInRow = jsvGetLength(buffer) / pixelRows;
  dataPnt = jswrap_espruino_getAddressOf(buffer,true);
  switch(direction){
    case 1: rotate_left(); break;
	case 2: rotate_right(); break;
    case 3: rotate_up(); break;
	case 4: rotate_down(); break;
  }
}
/*JSON{
  "type" : "staticmethod",
  "class" : "LedBoard",
  "name" : "reColor",
  "generate" : "jswrap_LedBoard_reColor",
  "params"   : [
    ["graphics","JsVar","graphics object"],
	["colors","JsVar","An array of new colors"],
	["frame", "JsVar", "frame (x1,y1,x2,y2,color)"]
  ]
}
sets all pins for LED Board
*/
void jswrap_LedBoard_reColor(JsVar *graphics,JsVar *colors,JsVar *frame){
  int i,x,y,val; char newColor[8];JsvObjectIterator it;
  if(jsvIsArray(colors)){
	i = 0;
	jsvObjectIteratorNew(&it,colors);
	while(jsvObjectIteratorHasValue(&it)){
      newColor[i] = jsvGetIntegerAndUnLock(jsvObjectIteratorGetValue(&it));
	  i++;
	  jsvObjectIteratorNext(&it);
	}
	jsvObjectIteratorFree(&it);
  }
  setFrameLoc(frame);
  JsVar *buffer = jsvObjectGetChild(graphics,"buffer",0);
  dataPnt = jswrap_espruino_getAddressOf(buffer,true);
  for(x = frameLoc.x1; x <= frameLoc.x2; x++){
    for(y = frameLoc.y1; y <= frameLoc.y2; y++){
      set_Color(x,y,newColor[get_Color(x,y)]);
	}
  }
}
/*JSON{
  "type" : "staticmethod",
  "class" : "LedBoard",
  "name" : "getFrame",
  "generate" : "jswrap_LedBoard_getFrame",
  "params"   : [
    ["graphics","JsVar","graphics object"],
	["frame", "JsVar", "frame (x1,y1,x2,y2,color)"]
  ],
  "return" : ["JsVar","colordata of select frame in an uint8array"]
}
reads color of given frame into an uint8array, be careful, needs a byte for each pixel
*/
JsVar *jswrap_LedBoard_getFrame(JsVar *graphics,JsVar *frame){
  JsVar *frameBuffer;int size,x,y;uint8_t *framePnt;
  JsVar *buffer = jsvObjectGetChild(graphics,"buffer",0);
  pixelsInRow = jsvGetLength(buffer) / pixelRows;
  dataPnt = jswrap_espruino_getAddressOf(buffer,true);
  setFrameLoc(frame);
  size = (frameLoc.x2 - frameLoc.x1 + 1) * (frameLoc.y2 - frameLoc.y1 + 1);
  frameBuffer = jswrap_arraybuffer_constructor(size);
  framePnt = jswrap_espruino_getAddressOf(frameBuffer,true);
  for(x = frameLoc.x1; x <= frameLoc.x2; x++){
    for(y = frameLoc.y1; y <= frameLoc.y2; y++){
      *framePnt = get_Color(x,y);
	  framePnt++;
    }
  }
  return frameBuffer;
}
/*JSON{
  "type" : "staticmethod",
  "class" : "LedBoard",
  "name" : "setFrame",
  "generate" : "jswrap_LedBoard_setFrame",
  "params"   : [
    ["graphics","JsVar","graphics object"],
	["frame", "JsVar", "frame (x1,y1,x2,y2,color)"],
	["frameData", "JsVar", "Framedata created by getFrame"]
  ]
}
set color of given frame into graphics buffer
*/
void jswrap_LedBoard_setFrame(JsVar * graphics, JsVar *frame, JsVar *frameData){
  int x,y;uint8_t *framePnt; 
  JsVar *buffer = jsvObjectGetChild(graphics,"buffer",0);
  pixelsInRow = jsvGetLength(buffer) / pixelRows;
  dataPnt = jswrap_espruino_getAddressOf(buffer,true);
  framePnt = jswrap_espruino_getAddressOf(frameData,true);
  setFrameLoc(frame);
  for(x = frameLoc.x1; x <= frameLoc.x2; x++){
    for(y = frameLoc.y1; y <= frameLoc.y2; y++){
      if(*framePnt != frameLoc.color) set_Color(x,y,*framePnt);
	  framePnt++;
    }
  }
}
#ifdef LEDBOARD_DEBUG
/*JSON{
  "type"     : "staticmethod",
  "class"    : "LedBoard",
  "ifdef"    : "LEDBOARD_DEBUG",
  "name"     : "debugInfo",
  "generate" : "jswrap_LedBoard_debugInfo"
}
*/
void jswrap_LedBoard_debugInfo(){
  LedBoardDebugInfo();
}
/*JSON{
  "type"     : "staticmethod",
  "class"    : "LedBoard",
  "ifdef"    : "LEDBOARD_DEBUG",
  "name"     : "testPin",
  "generate" : "jswrap_LedBoard_testPin",
  "params"   : [
    ["pin", "pin", "IO-Pin"],
	["value", "bool", "value"]
  ]
}
*/
void jswrap_LedBoard_testPin(Pin pin,bool value){
  LedBoardtestPin(pin,value);
}
/*JSON{
  "type"     : "staticmethod",
  "class"    : "LedBoard",
  "ifdef"    : "LEDBOARD_DEBUG",
  "name"     : "pulsePin",
  "generate" : "jswrap_LedBoard_pulsePin",
  "params"   : [
    ["pin", "pin", "IO-Pin"],
	["value", "bool", "value"]
  ]
}
*/
void jswrap_LedBoard_pulsePin(Pin pin,bool value){
  LedBoardtestPin(pin,value);
}
/*JSON{
  "type"     : "staticmethod",
  "class"    : "LedBoard",
  "ifdef"    : "LEDBOARD_DEBUG",
  "name"     : "testAddress",
  "generate" : "jswrap_LedBoard_testAddress",
  "params"   : [
    ["repeat", "int", "how often"]
  ]
}
*/
void jswrap_LedBoard_testAddress(int repeat){
  int r,i;
  for(r = 0; r < repeat; r++){
    for(i = 0; i < pixelRows; i++){
	  LedBoardSetEnable(true);
      LedBoardSetAddress(i);
	  LedBoardPulseLatch(); //pulse to high and delay
	  LedBoardSetEnable(false);
	}
	LedBoardSetDefault();
  }	  
}
/*JSON{
  "type"     : "staticmethod",
  "class"    : "LedBoard",
  "ifdef"    : "LEDBOARD_DEBUG",
  "name"     : "testRGBData",
  "generate" : "jswrap_LedBoard_testRGBData",
  "params"   : [
    ["graphics", "JsVar", "graphics Object"],
    ["repeat", "int", "how often"]
  ]
}
*/
void jswrap_LedBoard_testRGBData(JsVar *graphics,int repeat){
  int r,i;uint8_t *dataPnt;
  JsVar *buffer = jsvObjectGetChild(graphics,"buffer",0);
  pixelsInRow = jsvGetLength(buffer) / pixelRows;
  dataPnt = jswrap_espruino_getAddressOf(buffer,true);
  for(r = 0; r < repeat; r++){
	dataPnt = jswrap_espruino_getAddressOf(buffer,true);
    for(i = 0; i < pixelRows; i++){
	  LedBoardSendRow(dataPnt);
	  dataPnt += pixelsInRow;
	}
	LedBoardSetDefault();
  }	
}
/*JSON{
  "type"     : "staticmethod",
  "class"    : "LedBoard",
  "ifdef"    : "LEDBOARD_DEBUG",
  "name"     : "debug",
  "generate" : "jswrap_LedBoard_debug",
  "params"   : [
    ["grapics", "JsVar", "graphicsObject"]
  ]
}
*/
void jswrap_LedBoard_debug(JsVar *graphics){
  JsVar *buffer;
  buffer = jsvObjectGetChild(graphics, "buffer", 0);
  uint8_t *dataPnt;
  dataPnt = jswrap_espruino_getAddressOf(buffer,true);
  jsWarn("pnt:%d",dataPnt);
}
/*JSON{
  "type"     : "staticmethod",
  "class"    : "LedBoard",
  "ifdef"    : "LEDBOARD_DEBUG",
  "name"     : "setPixel",
  "generate" : "jswrap_LedBoard_setPixel",
  "params"   : [
    ["graphics", "JsVar", "Graphics Object"],
    ["x", "int", "X"],
	["y", "int", "Y"],
	["color", "int", "Color"]
  ]
}
*/
void jswrap_LedBoard_setPixel(JsVar *graphics,int x, int y, int color){
  JsVar *buffer = jsvObjectGetChild(graphics,"buffer",0);
  dataPnt = jswrap_espruino_getAddressOf(buffer,true);
  set_Color(x,y,color);
}
/*JSON{
  "type"     : "staticmethod",
  "class"    : "LedBoard",
  "ifdef"    : "LEDBOARD_DEBUG",
  "name"     : "getPixel",
  "generate" : "jswrap_LedBoard_getPixel",
  "params"   : [
    ["graphics", "JsVar", "Graphics Object"],
    ["x", "int", "X"],
	["y", "int", "Y"]
  ]
}
*/
void jswrap_LedBoard_getPixel(JsVar *graphics,int x, int y){
  JsVar *buffer = jsvObjectGetChild(graphics,"buffer",0);
  dataPnt = jswrap_espruino_getAddressOf(buffer,true);
  get_Color(x,y);
}
#endif

