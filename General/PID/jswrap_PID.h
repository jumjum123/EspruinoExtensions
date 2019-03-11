#ifndef JSWRAP_PID_H_
#define JSWRAP_PID_H_
#include "jsvar.h"
#endif
typedef struct {
  float kp,pi,kd;

} PACKED_FLAGS JsPidData;
typedef struct JsPid {
  JsVar *PidVar;
  JsPidData data;
} PACKED_FLAGS JsPid;
