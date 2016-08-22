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
 * Everything is based on Espruino Source from Gordon Williams
 * ----------------------------------------------------------------------------
 */

#include "jswrap_FlashExtension.h"
#include "jshardware.h"
#include "jswrap_flash.h"
#ifdef ESP8266
  int mapAddr = 0x40200000;
#else
  int mapAddr = 0x0;
#endif
int files;
int isMappedDir(int addr){
  int temp;
  jshFlashRead(&temp,(uint32_t)(addr),4);
  if((temp >> 16) == 0x4A4D)return (temp & 0xffff); else return 0;
}
int idByJsVar(int addr,JsVar *fileVar){
  int files,i,strAddr,len,temp;
  files = isMappedDir(addr);
  if(files > 0){
	if(jsvIsInt(fileVar)){
	  i = jsvGetInteger(fileVar);
	  if(i >= files) i = -1;
	  return i;
	}
	else{
      for(i = 0; i < files; i++){
	    strAddr = addr + 8 + i * 16;
	    jshFlashRead(&temp,(uint32_t)(strAddr),4);
	    strAddr = mapAddr + strAddr + 2;
	    len = (temp & 0xffff);
	    JsVar *v = jsvNewWithFlags(JSV_NATIVE_STRING);
	    v->varData.nativeStr.ptr = (char*)strAddr;
	    v->varData.nativeStr.len = (uint16_t)len;
	    temp = jsvCompareString(fileVar,v,0,0,false);
	    jsvUnLock(v);
        if(temp == 0) return i;
      }
	}
  }
  return -1;
}

/*JSON{
  "type" : "staticmethod",
  "class" : "Flash",
  "name" : "mappedString",
  "generate" : "jswrap_flash_mappedString",
  "params" : [
    ["addr","int","Start of page"],
	["fileVar","JsVar","Filename or Id"]
  ],
  "return" : ["JsVar","Mapped String"],
  "return_object" : "String"

}
*/
JsVar *jswrap_flash_mappedString(int addr, JsVar *fileVar){
  int i,strAddr, len, tmp;
  i = idByJsVar(addr,fileVar);
  if(i >= 0){
	jshFlashRead(&tmp,(uint32_t)(addr + i * 16 + 4),4);
	strAddr = (tmp >> 16) + mapAddr + addr;
	len = (tmp & 0xffff);
	JsVar *v = jsvNewWithFlags(JSV_NATIVE_STRING);
	v->varData.nativeStr.ptr = (char*)strAddr;
	v->varData.nativeStr.len = (uint16_t)len;
	return v;
  }
  else return 0;
}

/*JSON{
  "type" : "staticmethod",
  "class" : "Flash",
  "name" : "mappedDir",
  "generate" : "jswrap_flash_mappedDir",
  "params" : [
    ["addr","int","Start of page where strings are stored"]
  ],
  "return" : ["JsVar","StringArray of files"]
}
*/
JsVar *jswrap_flash_mappedDir(int addr){
  int files; int i; JsVar *arr = 0;
  int strAddr, len, tmp;
  files = isMappedDir(addr);
  if(files > 0){
	arr = jsvNewEmptyArray();
	for(i = 0; i < files;i++){
	  strAddr = addr + 8 + i * 16;
	  jshFlashRead(&tmp,(uint32_t)(strAddr),4);
	  strAddr = mapAddr + strAddr + 2;
	  len = (tmp & 0xffff);
	  JsVar *v = jsvNewWithFlags(JSV_NATIVE_STRING);
	  v->varData.nativeStr.ptr = (char*)strAddr;
	  v->varData.nativeStr.len = (uint16_t)len;
	  jsvArrayPush(arr, v);
      jsvUnLock(v);
	}
  }
  return arr;
}

/*JSON{
  "type" : "staticmethod",
  "class" : "Flash",
  "name" : "mappedArray",
  "generate" : "jswrap_flash_mappedArray",
  "params" : [
    ["addr","int","Start of page where strings are stored"],
    ["fileVar","JsVar","Filename"]
  ],
  "return" : ["JsVar","A Uint8Array"]
}
*/
JsVar *jswrap_flash_mappedArray(int addr, JsVar *fileVar){
  int i, strAddr, len, tmp;
  i = idByJsVar(addr,fileVar);
  if(i >= 0){
    jshFlashRead(&tmp, (uint32_t)(addr + i * 16 + 4), 4);
    strAddr = tmp >> 16;
    len = tmp & 0xffff;
    return jswrap_flash_read(len,addr + strAddr);
  }
  else return 0;
}
