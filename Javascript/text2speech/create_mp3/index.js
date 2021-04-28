const createMP3 = require('./generalFunctions.js');
createMP3.lang = 'de';
createMP3.convert("1#2#3#4#5#6#7#8#9#10",function(){
  createMP3.namingById = 1000;
  createMP3.convert("hundert#tausend",function(){console.log("done");});
});
