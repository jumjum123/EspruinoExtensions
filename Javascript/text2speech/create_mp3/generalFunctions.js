const gTTS = require('gtts');

function doSerial(data,fnc,callback){
  var i = 0;
  fnc(i,data[i],nextStep);
  function nextStep(){
    i++;
	if(i < data.length) fnc(i,data[i],nextStep);
	else callback();
  }
}
function text2mp3(){
  var me = this;
  me.namingById = true;
  me.lang = 'en';
  function fullNumber(nr){
    var txt = "";
	if(nr < 10) txt += "000" + nr;
	else if(nr < 100) txt += "00" + nr;
	else if(nr < 1000) txt += "0" + nr;
	else txt += nr;
	return txt;
  }
  function fileName(i,part){
	var txt = "./mp3_" + me.lang + "/";
    switch(me.namingById){
      case true: txt += fullNumber(i + 1); break;
      case false: txt += part; break;
      default: txt += fullNumber((me.namingById + i + 1)); break;  
	}
	return txt + ".mp3";
  }
  function convertPart(i,part, callback){
    var gtts = new gTTS(part,me.lang);
	gtts.save(fileName(i,part),function(err,result){
      if(err) throw new Error(err);
	  callback();
	});
  }
  me.convert = function(txt,callback){
	doSerial(txt.split("#"),convertPart,callback);
  };
}
module.exports = new text2mp3();
