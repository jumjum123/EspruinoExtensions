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
 
#include "jswrap_GraphicsExtension.h"
 
void graphicsCircle(JsGraphics *gfx, int x0, int y0, int radius,int fill){
  int x,y,err,e2;
  x = -radius, y = 0, err = 2-2 * radius;
  while(x <=0){
    if(fill == 1){
      graphicsFillRect(gfx,x0 - x,y0 - y,x0 - x,y0 + y);
      graphicsFillRect(gfx,x0 + x,y0 - y,x0 + x,y0 + y);  
    }
    else{
      graphicsSetPixel(gfx,x0-x, y0+y,gfx->data.fgColor);
      graphicsSetPixel(gfx,x0+x, y0+y,gfx->data.fgColor);
      graphicsSetPixel(gfx,x0+x, y0-y,gfx->data.fgColor);
      graphicsSetPixel(gfx,x0-x, y0-y,gfx->data.fgColor);    
    } 
    e2 = err;
    if (e2 <= y) {
      err += ++y*2+1;
      if (-x == y && e2 <= x) e2 = 0;
    }
    if (e2 > x) err += ++x*2+1;
  }
}
/*JSON{
  "type" : "method",
  "class" : "Graphics",
  "name" : "fillCircle",
  "generate" : "jswrap_graphics_fillCircle",
  "params" : [
    ["x","int32","Center X"],
    ["y","int32","Center Y"],
    ["radius","int32","Radius"]
  ]
}
Fill a circle in the Foreground Color
*/
void jswrap_graphics_fillCircle(JsVar *parent, int x, int y, int radius) {
  JsGraphics gfx; if (!graphicsGetFromVar(&gfx, parent)) return;
  graphicsCircle(&gfx, x,y,radius,1);
  graphicsSetVar(&gfx); // gfx data changed because modified area
}
/*JSON{
  "type" : "method",
  "class" : "Graphics",
  "name" : "fillCircle",
  "generate" : "jswrap_graphics_drawCircle",
  "params" : [
    ["x","int32","Center X"],
    ["y","int32","Center Y"],
    ["radius","int32","Radius"]
  ]
}
Draw a circle in the Foreground Color
*/
void jswrap_graphics_drawCircle(JsVar *parent, int x, int y, int radius) {
  JsGraphics gfx; if (!graphicsGetFromVar(&gfx, parent)) return;
  graphicsCircle(&gfx, x,y,radius,0);
  graphicsSetVar(&gfx); // gfx data changed because modified area
}