function text2SpeechCmd(uart){
  var me = this;me.debug = false;
  var cb, idx = 0, fdb = Uint8Array(10);
  function dbg(hex){
    if(me.debug){
      var t = "";
      if(hex[3] == 0x40) t +="Error " + hex[6].toString(16);
      else {
        for(var i = 0; i < hex.length; i++) t += hex[i].toString(16) + " ";
      }
      console.log(t);
    }
  }
  function doCmd(comand,dataHigh,dataLow){
    var chksum,cmd = Uint8Array(10);
    cmd[0] = 0x7e;cmd[1] = 0xff; cmd[2] = 6;cmd[4] = 0; cmd[9] = 0xef;
    cmd[3] = comand; cmd[5] = dataHigh; cmd[6] = dataLow;
    chksum = 0xffff -(cmd[1] + cmd[2] + cmd[3] + cmd[4] + cmd[5] + cmd[6]) + 1;
    cmd[7] = (chksum>>8) & 0xff; cmd[8] = chksum & 0xff;
    idx = 0;fdb.fill(0);
    dbg(cmd);
    uart.write(cmd);
  }
  function init(){
    uart.setup(9600);
    uart.on('data',function(data){
      fdb[idx] = data.charCodeAt(0);
      if(fdb[idx] == 239) cb(fdb);
      idx++;
    });
  }
  me.play = function(dir,file,callback){
    if(callback) cb = callback; else cb = dbg;
    switch(dir){
      case 0: doCmd(0x12,0,file,cb); break;
      case 99: doCmd(0x13,0,file,cb); break;
      default: if(dir>0 & dir < 13) doCmd(0x14,(dir<<4) + (file>>8),file & 0xff,cb);
    }
  };
  me.volume = function(vol,callback){
    if(callback) cb = callback; else cb = dbg;
    if(vol > 0) doCmd(6,0,vol,cb);
    else doCmd(0x43,0,0,cb);
  };
  me.playMulti = function(files,timeBetween,callback){
    var idx = 0;
    if(!timeBetween) timeBetween = 10;
    function next(){
      idx++;
      if(idx < files.length){
        setTimeout(function(){
          me.play(files[idx][0],files[idx][1],next);
        },timeBetween);
      }
      else {if(callback) callback();}
    }
    if(files.length > 0) me.play(files[idx][0],files[idx][1],next);
    else next();
  };
  init();
}
var t = new text2SpeechCmd(Serial2);
//t.volume(30);
//t.playMulti([[1,1],[1,3],[1,5],[1,7],[1,9]],10,function(){console.log("done");});
//t.play(0,1);
//t.playMulti([[1,2],[1,102],[1,24],[2,13]],10,function(){console.log("done");});