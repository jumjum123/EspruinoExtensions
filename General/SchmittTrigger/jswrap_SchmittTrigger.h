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
 * Everything is based on Espruino Source from Gordon Williams
 * ----------------------------------------------------------------------------
 */
#include "jsvar.h"

#ifndef TARGETS_JSWRAP_SCHMITTTRIGGER_H_
#define TARGETS_JSWRAP_SCHMITTTRIGGER_H_

typedef struct {
  float low,high;
  bool status;
} PACKED_FLAGS JsSchmittTriggerData;
typedef struct {
  JsVar *SchmittTriggerVar;
  JsSchmittTriggerData data;
} PACKED_FLAGS JsSchmittTrigger;

JsVar *jswrap_SchmittTrigger_constructor(JsVarFloat low,JsVarFloat high);
bool jswrap_SchmittTrigger_set(JsVar *parent,JsVarFloat value);

#endif /* TARGETS_JSWRAP_SCHMITTTRIGGER_H_ */
