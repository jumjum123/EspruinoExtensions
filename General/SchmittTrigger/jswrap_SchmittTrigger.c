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

#include "jswrap_SchmittTrigger.h"
#include "jsparse.h"
#include "jsinteractive.h"
#include "jsutils.h" 
 
static inline void structInit(JsSchmittTrigger *st,JsVarFloat low,JsVarFloat high){
  st->data.low = low;
  st->data.high = high;
  st->data.status = false;
}
void setVar(JsSchmittTrigger *st){
  JsVar *dataname = jsvFindChildFromString(st->SchmittTriggerVar,JS_HIDDEN_CHAR_STR"st",true);
  JsVar *data = jsvSkipName(dataname);
  if (!data) {
    data = jsvNewStringOfLength(sizeof(JsSchmittTriggerData),0);
    jsvSetValueOfName(dataname, data);
  }
  jsvUnLock(dataname);
  assert(data);
  jsvSetString(data, (char*)&st->data, sizeof(JsSchmittTriggerData));
  jsvUnLock(data);
}
bool getFromVar(JsSchmittTrigger *st, JsVar *parent){
  st->SchmittTriggerVar = parent;
  JsVar *data = jsvObjectGetChild(parent,JS_HIDDEN_CHAR_STR"st",0);
  assert(data);
  if(data){
    jsvGetString(data, (char*)&st->data, sizeof(JsSchmittTriggerData)+1);
    jsvUnLock(data);
    return true;
  } else
    return false;
}
void emitOnChange(JsVar *parent,bool status){
  JsVar *args[1];
  args[0] = jsvNewFromBool(status);
  JsVar *eventName = jsvNewFromString(JS_EVENT_PREFIX"change");
  JsVar *callback = jsvSkipNameAndUnLock(jsvFindChildFromVar(parent, eventName, 0));
  jsvUnLock(eventName);
  if (callback) jsiQueueEvents(parent, callback, args, 1);
  jsvUnLock(callback);
  jsvUnLockMany(1, args);
}

/*JSON{
  "type"  : "class",
  "class" : "SchmittTrigger"
}
This is the built-in class for Schmitt Trigger
*/
/*JSON{
  "type" : "event",
  "class" : "SchmittTrigger",
  "name" : "onChanged"
}
This event is called when status of Schmitt Trigger changes.

You can use this for logging potential problems that might occur during execution.
*/
/*JSON{
  "type"     : "constructor",
  "class"    : "SchmittTrigger",
  "name"     : "SchmittTrigger",
  "generate" : "jswrap_SchmittTrigger_constructor",
  "params"   : [
    ["low", "float", "level to switch status to low"],
	["high", "float", "level to switch status to high"]
  ],
  "return"   : ["JsVar","A SchmittTrigger object"]
}
Creates a SchmittTrigger with given levels
*/
JsVar *jswrap_SchmittTrigger_constructor(JsVarFloat low,JsVarFloat high){
  JsVar *parent = jspNewObject(0,"SchmittTrigger");
  JsSchmittTrigger st;
  structInit(&st,low,high);
  st.SchmittTriggerVar = parent;
  setVar(&st);
  return parent;
}

/*JSON{
  "type"     : "method",
  "class"    : "SchmittTrigger",
  "name"     : "set",
  "generate" : "jswrap_SchmittTrigger_set",
  "params"   : [
    ["value", "float", "Calculates new state based on value and last state"]
  ],
  "return"   : ["bool","new status of Schmitt Trigger based on value and actual state"]
  
}
Calculates new state based on value and last state
 */
bool jswrap_SchmittTrigger_set(JsVar *parent, JsVarFloat value){
  JsSchmittTrigger st;
  if(!getFromVar(&st,parent)) return 0;
  if(st.data.status == true){
	if(value < st.data.low){
	  st.data.status = false;
	  emitOnChange(parent,false);
	}
  }
  else{
	if(value > st.data.high){
	  st.data.status = true;
	  emitOnChange(parent,true);
	}	  
  }
  setVar(&st);
  return st.data.status;
}