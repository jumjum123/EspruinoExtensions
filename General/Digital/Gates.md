Digital Gates
=====================================
May be some of you remember decades ago, those dark times, where we had no Internet, no Computer. This was some years after the invention of transistors. A small light in the dark popped up and we saw IC's, Integrated circuits.
Beautiful names like SN7400 or CD4011 appeared, and first people started to build their own digital watches and other incredible things.
Nowaday we talk about IOT, BT, AMD and our phones have more power than a SuperComputer that time.
But here is help, Digital Gates are built to bring back those good old times to all of us ;-)

During this following extensions and some ++__[tutorials](http://jumspruino.jumware.com/docu/Extensions/CreateEspruinoExtensions.html/)__++ have been created.

####Class Digital####
Digital is a head class, which helps to create Digital Gates
- **Digital.AND** Constant, used in DigitalGate to create a Gate
- **Digital.NAND**
- **Digital.OR**
- **Digital.NOR**
- **Digital.EXOR**
- **Digital.FLIPFLOP**
- **Digital.ROTATE**

####Class DigitalGate####
See file testgates.js as an example.
- **new** Creates Gates
 - **AND** g = new DigitalGate(Digital.AND,{inp:[0,1,1...]})
simple AND gate,Out is set to true if all values are true.
   - **inp** inp is an array of binary inputs. There is no limit for number of inputs, as we had in ICs. You could easily do this with an javascript statement, but come on, thats not what you really want, don't you.
Binary inputs can be of different types
     - *binary*
     - *number*
     - an other DigitalGate
     - a function which returns true or false
     - a PIN of your board
 - **NAND** g = new DigitalGate(Digital.NAND,{inp:[1,1,0...]})
simple NOT-AND Gate, out is set to false if all inputs are true
   - **inp** see AND
 - **OR** g = new DigitalGate(Digital.OR,{inp:[0,0,1...]})
simple OR Gate, out is set to true, if at least one input is true
   - **inp** see AND
 - **NOR** g = new DigitalGate(Digital.NOR,{inp:[0,1,0...]})
simple NOT-OR Gate, out is set to false if at least on input is true
   - **inp** see AND
 - **EXOR** g = new DigitalGate(Digital.EXOR,{inp:[0,0,1,1..]})
simple EXOR Gate, out is set to true if inputs are not all the same
   - **inp** see AND
 - **FLIPLOP** g = new DigitalGate(Digital.FLIPFLOP,{set:0,reset:0})
simple flipflop, out is set to true and stays true if set is true
out is set to false and stays false if reset is true
 - **ROTATE** g = new DigitalGate(Digital.ROTATE,{inp:[0,1,1,...]})
Rotates through inp and sets out according to actual value in inp. 
- **execute** g.execute()
Execute the gate. Good old IC did this all the time, but thats not as easy in software. May be we can use the idle loop in Espruino in a later version ??
- **out** g.out
Output value of gate. This value is one bit only, true or false. Good old ICs like 7490 are not (yet?) in focus
- **edge** gate.on("edge",function(edge){ your code here })
Edge is an event which is set on change of out. Paramter shows falling or raising edge
 - *fall* out value changed from true to false
 - *raise* out value changed from false to true
