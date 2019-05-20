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

#ifndef TARGETS_JSWRAP_GATES_H_
#define TARGETS_JSWRAP_GATES_H_

bool jswrap_Digital_idle();
bool jswrap_Digital_value(JsVar *port);

JsVar *jswrap_DigitalGate_constructor(int type,JsVar *arr);
bool jswrap_DigitalGate_execute(JsVar *parent);

#endif /* TARGETS_JSWRAP_GATES_H_ */
