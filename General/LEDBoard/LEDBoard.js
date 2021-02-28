require("Font8x16").add(Graphics);
require("Font6x8").add(Graphics);
var grf,fl; const defBoard = "ESP32";
var w = 64,h = 32,board;
function fileHandler(){
  var me = this;
  var fl,board;
  me.init = function(brd){
    board = brd;
    switch(board){
      case "ESP32": fl = request("storage"); break;
      case "Espruino": fl = require("fs"); break;
    }
  };
  me.list = function(){
    switch(board){
      case "ESP32": return fl.list();
      case "Espruino": return fl.readdir();
    }
  };
  me.read = function(fileName){
    switch(board){
      case "ESP32": return fl.readArrayBuffer(fileName);
      case "Espruino": return E.toUint8Array(fl.readFile(fileName));
    }
  };
}
function init(size,brd){
  if(!size) size = 1024;
  if(!brd) board = defBoard; else board = brd;
  fl = new fileHandler();
  fl.init(board);
  switch(size){
    case 512: w = 32;h = 32; break;
    case 1024: w = 64; h = 32; break;
    case 2048: w = 64; h = 64; break;
    case 4096: w = 128; h = 64; break;
  }
  grf = Graphics.createArrayBuffer(w,h,4,{interleavex:true});
  switch(board){
    //R1,R2,G1,G2,B1,B2  A,B,C,D(,E)?  Enable,Latch,Clock
    case "Espruino":
      if(h > 32)
        LedBoard.init([B3,B5,C11,C10,B4,B6],[B7,C9,B8,C8,B12],[C12,C6,B9]);
      else
        LedBoard.init([B3,B5,C11,C10,B4,B6],[B7,C9,B8,C8],[C12,C6,B9]);
      break;
    case "ESP32":
      if(h > 32)
        LedBoard.init([D2,D14,D15,D27,D4,D12],[D21,D22,D19,D23,D18],[D25,D26,D5]);
      else
        LedBoard.init([D2,D14,D15,D27,D4,D12],[D21,D22,D19,D23],[D25,D26,D5]);
      break;
  }
}
function start(){
  setInterval(function(){LedBoard.send(grf);},20);
}
function stop(){
  clearInterval();
  grf.setBgColor(0);
  grf.clear();
}
//Some demos
function color(col){
  stop();
  clearInterval();
  if(!col) col = 0;
  grf.setBgColor(col);
  grf.clear();
  start();
}
function txt(t,c){
  stop();
  if(!t) t = "Open";
  if(!c) c = 2;
  var cbg = 7;
  function frame(cbg){
    grf.setBgColor(0);
    grf.clear();
    grf.setColor(cbg);
    grf.drawRect(0,0,w - 1,h - 1);
    grf.drawRect(1,1,w - 2,h - 2);
  }
  frame(cbg);
  grf.setColor(cbg);
  grf.drawEllipse(2,2,w - 2,h - 2);
  //grf.setFont8x16();
  if(w > 32) grf.setFont("8x16",2); else grf.setFont("8x16",1);
  grf.setColor(c);
  grf.drawString(t,w / 2 - grf.stringWidth(t) / 2,7);
  start();
  setInterval(function(){
    cbg--;if(cbg<1) cbg=7;
    grf.setColor(cbg);grf.drawEllipse(2,2,w - 2,h - 2);
    grf.setColor(8 - cbg);
    grf.drawString(t,w / 2 - grf.stringWidth(t) / 2,7);
  },500);
}
function graph(){
  stop();
  var barh = 5;
  grf.setBgColor(6);
  grf.clear();
  grf.setColor(2);
  grf.fillRect(5,h / 3,w - 10,h * 2 / 3);
  grf.setColor(4);
  grf.fillRect(10,14,40,20);
  grf.fillRect(10,0,40,0);
  grf.setColor(7);
  grf.drawRect(0,h - 1,5,5);
  start();
  setInterval(function(){
    barh++; if(barh > (h - 8)) barh = 1;
    grf.setColor(0);
    grf.fillRect(1,h - 2,4,6);
    grf.setColor(1);
    grf.fillRect(1,h - 2,4,h - 2 - barh);
  },200);
}
function magic(){
  var col = 1;radius = h / 2 - 1;
  stop();
  grf.setBgColor(0);
  grf.clear();
  start();
  setInterval(function(){
    col++;
    if(col>7) col = 1;
    radius--;
    if(radius < 3){
      radius = h / 2 - 1;
      grf.setColor(0);
      grf.fillCircle(h / 2,h / 4,h / 4);
    }
    grf.setColor(col);
    grf.fillCircle(h / 2 - 2,h / 2 - 1,radius);
  },50);
}
function mondrinator(){
  var rndCol = function(){ return (Math.random() * 6 + 1);};
  var rndBorder = function(){ return 1 + Math.random() * h / 5;};
  var rndRect = function(){ return Math.random() * 4;};
  function showNext(){
    var right,left,top,bottom,rectId;
    grf.setBgColor(0);grf.clear();
    grf.setColor(rndCol());left = rndBorder();grf.drawLine(left,0,left,h - 1);
    grf.setColor(rndCol());right = rndBorder();grf.drawLine(w-right-1,0,w-right-1,h - 1);
    grf.setColor(rndCol());top = rndBorder();grf.drawLine(0,top,w - 1,top);
    grf.setColor(rndCol());bottom = rndBorder();grf.drawLine(0,h - bottom - 1,w - 1,h - 1 -bottom);
    rectId = parseInt(rndRect());grf.setColor(rndCol());
    switch(rectId){
      case 0: grf.fillRect(0,0,left - 1,top - 1);break;
      case 1: grf.fillRect(w-right,0,w - 1,top - 1);break;
      case 2: grf.fillRect(0,h - bottom,left-1,h - 1);break;
      case 3: grf.fillRect(w-right,h - 1,w - 1,h - bottom);break;
    }
    setTimeout(function(){showNext();},5000);
  }
  stop();start();
  showNext();
}
function cubesArt(){
  var i,cubes = [],cnt = 20,dirs;
  function createCubes(){
    cubes = [];
    for(i = 0; i < cnt; i++){
      cubes.push({
        val: parseInt(Math.random() * 100 + 16),
        col: (i % 7) + 1
      });
    }
  }
  function createDirs(){
    dirs = [];
    for(i = 0; i < 8; i++){
      dirs.push("TBLR".substr(parseInt(Math.random() * 4),1));
    }
  }
  function calcCubes(){
    var sumCube = 0,sizeW = w,sizeH = h,pixel = sizeW * sizeH;
    var top = 0,left = 0, width = sizeW, height = sizeH;
    for(i = 0; i < cnt; i++) sumCube += cubes[i].val;
    for(i = 0; i < cnt; i++){
      var cx,cy,cw,ch,d,v;
      v = cubes[i].val;
      d = dirs[i % 4];
      switch(d){
        case "T":
          cx = left; cy = top;
          cw = width; ch = Math.round(v / sumCube / width * pixel);
          top = top + ch; height = height - ch;
          break;
        case "B":
          cx = left; ch = Math.round(v / sumCube / width * pixel);
          cw = width; cy = top - ch + height; height = height - ch;
          break;
        case "L":
          cx = left; cy = top;
          cw = Math.round(v / sumCube / height * pixel); ch = height;
          left = left + cw; width = width - cw;
          break;
        case "R":
          cy = top; cw = Math.round(v / sumCube / height * pixel);
          cx = left + width - cw; ch = height;
          width = width - cw;
          break;
      }
      cubes[i].x = cx;
      cubes[i].y = cy;
      cubes[i].w = cw;
      cubes[i].h = ch;
    }
  }
  function drawCubes(){
    var cube;
    grf.setBgColor(7);grf.clear();
    for(i = 0; i < cnt; i++){
      cube = cubes[i];
      grf.setColor(cube.col);
      grf.fillRect(cube.x,cube.y,cube.x + cube.w,cube.y + cube.h);
    }
  }
  function showNext(){
    createCubes();
    createDirs();
    calcCubes();
    drawCubes();
    setTimeout(function(){showNext();},5000);
  }
  stop();start();
  showNext();
}
function pie(){
  var i,last;
  stop();
  last = 0;
  centerX = w * 2 / 3; centerY = h / 2;
  var data = [[20,1,"Apple"],[30,2,"Beef"],[40,4,"Beer"],[10,6,"Pills"]];
  function drawPie(pnt){
    var rad = h / 2 - 1;
    grf.setColor(7);
    //grf.drawLine(0,0,0,h - 1);
    //grf.drawLine(1,0,1,h - 1);
    var i,dt = data[pnt];
    var polyData = [centerX,centerY];
    var s = Math.PI * last / 100;
    var x = centerX + Math.round(Math.cos(s) * rad);
    var y = centerY + Math.round(Math.sin(s) * rad);
    polyData.push(x); polyData.push(y);
    for(i = 0; i < 10; i++){
      last += dt[0] / 10;
      s = Math.PI * (last + dt[0] / 10 * i) / 100;
      x = centerX + Math.round(Math.cos(s) * rad);
      y = centerY + Math.round(Math.sin(s) * rad);
      polyData.push(x);polyData.push(y);
    }
    last += dt[0];
    grf.setColor(dt[1]);
    grf.fillPoly(polyData,true);
  }
  function drawText(pnt){
    var dt = data[pnt];
    grf.setFont6x8();
    grf.setColor(dt[1]);
    grf.drawString(dt[2],4,pnt * 8);
  }
  for(i = 0; i < data.length;i++){
    drawPie(i);
    drawText(i);
  }
  start();
}

function xmas(){
  var pnt = 0;
  var px = [[10,10],[25,20],[30,15],[40,14]];
  function flicker(){
    grf.setPixel(px[pnt][0],px[pnt][1],2);
    pnt++;if(pnt>3)pnt = 0;
    grf.setPixel(px[pnt][0],px[pnt][1],7);
  }
  stop();
  function frame(){
    grf.setBgColor(0);
    grf.clear();
    grf.setColor(7);
    grf.drawRect(0,0,w - 1,h - 1);
    grf.drawRect(1,1,w -2,h - 2);
  }
  frame();
  grf.setColor(2);
  grf.fillPoly([1,1,1,31,30,16,1,1]);
  grf.fillPoly([15,3,15,28,40,16,15,3]);
  grf.fillPoly([28,6,28,25,50,16,28,6]);
  grf.setColor(7);
  grf.drawLine(6,2,8,2);
  grf.drawLine(6,30,8,30);
  start();
  setInterval(function(){flicker();},300);
}
function files(){
  var files,pnt = 0;
  function showNext(){
    stop();
    (new Uint8Array(grf.buffer)).set(fl.read(files[pnt]));
    start();
    pnt++;
    if(pnt >= files.length) pnt = 0;
    setTimeout(showNext,5000);
  }
  files = fl.list();
  stop();start();
  showNext();
}
function file(pnt){
  var files = fl.list();
  var cols = [0,1,2,3,4,5,6,7];
  if(!pnt) pnt = 0;
  function disorder() {
    var i, pos, temp;
    for (i = 0; i < 10; i++) {
      pos = Math.random() * cols.length | 0;
      temp = cols[pos];
      cols.splice(pos, 1);
      cols.push(temp);
    }
  }
  function showNext(){
    disorder();
    LedBoard.reColor(grf,cols);
    setTimeout(showNext,3000);
  }
  files = st.list();
  stop();
  (new Uint8Array(grf.buffer)).set(fl.read(files[pnt]));
  start();
  setTimeout(showNext,3000);
}

//Testing LedBoard
function simple(size){init(size);pie();}
function rotate(frame){
  var i,itv;
  function tst(){
    pie();
    itv = setInterval(function(){
      if(frame) LedBoard.rotate(grf,i,frame);
      else LedBoard.rotate(grf,i);
    },50);
    setTimeout(function(){
      clearInterval(itv);
      i++;
      if(i < 5) tst(); else console.log("test done");
    },1000);
  }
  i = 1;
  tst();
}
function rotatePart(){
  rotate({x1:20,y1:0,x2:60,y2:30});
}
function scroll(){
  rotate({x1:20,y1:7,x2:50,y2:15,color:3});
}
function recolor(){
  var frame = {x1:28,y1:0,x2:60,y2:30};
  var newCol = [0,6,5,4,3,2,1,7];
  pie();
  setTimeout(function(){
    LedBoard.reColor(grf,newCol);
    setTimeout(function(){
      LedBoard.reColor(grf,newCol);
      setTimeout(function(){
        LedBoard.reColor(grf,newCol,frame);
        setTimeout(function(){
          LedBoard.reColor(grf,newCol,frame);
          console.log("testcolor done");
        },2000);
      },2000);
    },2000);
  },2000);
}
function frames(){
  var tmp,frame = {x1:40,y1:10,x2:50,y2:22};
  pie();
  tmp = LedBoard.getFrame(grf,frame);
  color(3);
  LedBoard.setFrame(grf,frame,tmp);
  frame.x1 -= 14;frame.x2 -=14;
  LedBoard.setFrame(grf,frame,tmp);
  frame.x1 -= 14;frame.x2 -= 14; frame.color = 4;
  LedBoard.setFrame(grf,frame,tmp);
  console.log("Using Frame done");
}

//simple(4096);
