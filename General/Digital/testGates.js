function logError(on){
  console.log("!!!! Error " + testing + " on " + on);
}
function logSuccess(on){
  console.log("success on " + testing);
}
function logEvent(on){
  console.log("Event " + on + " on " + testing);
}
var testing;
function testAND(){
  testing = "AND";
  var gate = new DigitalGate(Digital.AND,{inp:[1,1,1]});
  if(gate.out === true){
    gate.args.inp = [0,1,1];
    if(gate.execute() === false){
      if(gate.edge == "fall"){
        gate.args.inp = [1,1,1];
        gate.execute();
		if(gate.edge == "raise"){
		  logSuccess();
		}
		else logError("edge raise");
	  }
	  else logError("edge fall");
    }
	else logError("out false");
  }
  else logError("out true");
}
function testNAND(){
  testing = "NAND";
  var gate = new DigitalGate(Digital.NAND,{inp:[1,1,1]});
  if(gate.out === false) logSuccess();
}
function testOR(){
  testing = "OR";
  var gate = new DigitalGate(Digital.OR,{inp:[1,0,0]});
  if(gate.out === true){
	gate.args.inp = [0,0,0];
	if(gate.execute() === false){
	  if(gate.edge == "fall"){
		gate.args.inp = [1,0,0];
		gate.execute();
		if(gate.edge == "raise"){
		  logSuccess();
		}
		else logError("edge raise");
	  }
	  else logError("edge fall");
	}
	else logError("out false");
  }
  else logError("out true");
}
function testNOR(){
  testing = "NOR";
  var gate = new DigitalGate(Digital.NOR,{inp:[1,0,0]});
  if(gate.out === false) logSuccess();
}
function testEvent(){
  var iv;
  testing = "Event";
  var gate = new DigitalGate(Digital.AND,{inp:[1,1,1]});
  gate.on("edge",function(a){
    logEvent(a);
	clearInterval(iv);
  });
  iv = setTimeout(function(){logError("edge fall");},100);
  gate.args.inp = [0,1,1];
  gate.execute();
  setTimeout(function(){
	iv = setTimeout(function(){logError("edge raise");},100);
	gate.args.inp = [1,1,1];
	gate.execute();
  },200);
}
function testEXOR(){
  testing = "EXOR";
  var gate = new DigitalGate(Digital.EXOR,{inp:[1,1,1]});
  if(gate.out === false){
	gate.args.inp = [0,1,1];
	if(gate.execute() === true) logSuccess();
	else logError("out true");
  }
  else logError("out false");
}
function testFLIPFLOP(){
  testing = "FLIPFLOP";
  var gate = new DigitalGate(Digital.FLIPFLOP,{set:0,reset:0});
  if(gate.out === false){
	gate.args.set = 1;
	gate.execute();
	if(gate.out === true){
	  gate.args.set = 0;
	  gate.execute();
	  if(gate.out === true){
		gate.args.reset = 1;
		gate.execute();
		if(gate.out === false){
		  logSuccess("done");
		}
		else logError("reset");
	  }
	  else logError("does not stay");
	}
	else logError("set");
  }
  else logError("start undefined");
}
function testROTATE(){
  testing = "ROTATE";f = true;
  var gate = new DigitalGate(Digital.ROTATE,{clk:0,data:0,inp:[1,0,1,0]});
  for(i = 0; i < 4; i++){
    if(gate.execute() != gate.args.inp[i]){logError(i); f = false;}
  }
  if(gate.execute() != gate.args.inp[0]){logError(0); f = false;}
  if(f) logSuccess();
}
function testCOUNTER(){
  testing = "COUNTER";
  var gate = new DigitalGate(Digital.COUNTER,{max:3});
}
function testInput(){
  testing = "testInputFnc";
  function returnBool(a){
    return true;
  }
  var gate2;
  var gate = new DigitalGate(Digital.AND,{inp:[1,returnBool,1]});
  if(gate.execute() === true){
    gate.args.inp[0] = 0;
    gate.execute();
    if(gate.out === false){
      logSuccess();
      testing = "testInputDigitalGate";
      gate2 = new DigitalGate(Digital.AND,{inp:[1,gate,1]});
      if(gate2.execute() === false){
        gate.args.inp[0] = 1;
        gate.execute();
        if(gate2.execute() === true) logSuccess();
        else logError("Gate true");
      }
      else logError("Gate false");    
    }
    else logError("function false");
  }
  else logError("function true");
}

testAND();
testNAND();
testOR();
testNOR();
testEXOR();
testFLIPFLOP();
testROTATE();
testInput();
testEvent();
