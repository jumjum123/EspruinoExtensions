/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
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
 * Contains ESP8266 board specific functions for Flash handling.
 * 
 * Everything is based on Espruino Source from Gordon Williams
 * ----------------------------------------------------------------------------
 */

#include "jswrap_FlashExtension.h"
#include "jshardware.h"

/*JSON{
  "type" : "staticmethod",
  "class" : "Flash",
  "name" : "readMappedArray",
  "generate" : "jswrap_flash_readMappedArray",
  "params" : [
    ["addr","int","Start of page where strings are stored"],
    ["pointer","int","seqNr of string"]
  ],
  "return" : ["JsVar","A Uint8Array"]
}
*/
JsVar *jswrap_flash_readMappedArray(int addr, int pointer){
  int strAddr, len, tmp;
  jshFlashRead(&tmp, (uint32_t)(addr + pointer * 4), 4);
  len = tmp >> 16;
  strAddr = tmp & 0xffff;
  return jswrap_flash_read(len,addr + strAddr);
}
/*JSON{
  "type" : "staticmethod",
  "class" : "Flash",
  "name" : "readMappedString",
  "generate" : "jswrap_flash_readMappedString",
  "params" : [
    ["addr","int","Start of page where strings are stored"],
    ["pointer","int","seqNr of string"]
  ],
  "return" : ["JsVar","A Uint8Array"],
  "return_object" : "String"
}
*/
JsVar *jswrap_flash_readMappedString(int addr, int pointer){
  int strAddr, len, tmp;
  jshFlashRead(&tmp, (uint32_t)(addr + pointer * 4), 4);
  len = tmp >> 16;
  strAddr += (tmp & 0xffff) + 0x40200000 + addr;
  JsVar *v = jsvNewWithFlags(JSV_NATIVE_STRING);
  v->varData.nativeStr.ptr = (char*)strAddr;
  v->varData.nativeStr.len = (uint16_t)len;
  return v; 
}
