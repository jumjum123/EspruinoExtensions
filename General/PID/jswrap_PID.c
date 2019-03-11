#include "jswrap_PID.h"

/*JSON{
  "type" : "class",
  "class" : "PID"
}
A class to support a simple PID controller
*/

/*JSON{
  "type" : "staticmethod",
  "class" : "PID",
  "name" : "init",
  "generate" : "jswrap_Pid_init",
  "params" :[
    ["input","JsVar","Input value"],
    ["output","JsVar","Output Value"],
	["setpoint","JsVar","Setpoint"]
  ]
}
Intializes Pid controler with Input, Output and Setpoint
*/
void jswrap_Pid_init(JsVar input,JsVar output,JsVar setpoint){

}

/*JSON{
  "type" : "staticmethod",
  "class" : "PID",
  "name" : "setParam",
  "generate" : "jswrap_Pid_setParam",
  "params" :[
    ["kp","float","KP"],
    ["kd","float","KI"],
	["kp","float","KD"]
  ]
}
Set controler parameter
*/
void jswrap_Pid_setParam(JsFloat kp,JsFloat ki,JsFloar kd){

}

/*JSON{
  "type" : "staticmethod",
  "class" : "PID",
  "name" : "setMode",
  "generate" : "jswrap_Pid_setMode",
  "params" :[
    ["automatic","binary","Mode"]
  ]
}
Set controler parameter
*/
void jswrap_Pid_setMode(binary mode){

}

unsigned long lastTime;
double Input, Output, Setpoint;
double ITerm, lastInput;
double kp, ki, kd;
int SampleTime = 1000; //1 sec
double outMin, outMax;
bool inAuto = false;
 
#define MANUAL 0
#define AUTOMATIC 1
 
void Compute()
{
   if(!inAuto) return;
   unsigned long now = millis();
   int timeChange = (now - lastTime);
   if(timeChange>=SampleTime)
   {
      /*Compute all the working error variables*/
      double error = Setpoint - Input;
      ITerm+= (ki * error);
      if(ITerm> outMax) ITerm= outMax;
      else if(ITerm< outMin) ITerm= outMin;
      double dInput = (Input - lastInput);
 
      /*Compute PID Output*/
      Output = kp * error + ITerm- kd * dInput;
      if(Output> outMax) Output = outMax;
      else if(Output < outMin) Output = outMin;
 
      /*Remember some variables for next time*/
      lastInput = Input;
      lastTime = now;
   }
}
 
void SetTunings(double Kp, double Ki, double Kd)
{
  double SampleTimeInSec = ((double)SampleTime)/1000;
   kp = Kp;
   ki = Ki * SampleTimeInSec;
   kd = Kd / SampleTimeInSec;
}
 
void SetSampleTime(int NewSampleTime)
{
   if (NewSampleTime > 0)
   {
      double ratio  = (double)NewSampleTime
                      / (double)SampleTime;
      ki *= ratio;
      kd /= ratio;
      SampleTime = (unsigned long)NewSampleTime;
   }
}
 
void SetOutputLimits(double Min, double Max)
{
   if(Min > Max) return;
   outMin = Min;
   outMax = Max;
    
   if(Output > outMax) Output = outMax;
   else if(Output < outMin) Output = outMin;
 
   if(ITerm> outMax) ITerm= outMax;
   else if(ITerm< outMin) ITerm= outMin;
}
 
void SetMode(int Mode)
{
    bool newAuto = (Mode == AUTOMATIC);
    if(newAuto && !inAuto)
    {  /*we just went from manual to auto*/
        Initialize();
    }
    inAuto = newAuto;
}
 
void Initialize()
{
   lastInput = Input;
   ITerm = Output;
   if(ITerm> outMax) ITerm= outMax;
   else if(ITerm< outMin) ITerm= outMin;
}