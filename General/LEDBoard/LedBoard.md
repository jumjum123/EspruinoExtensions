LedBoard support for LED-Panels
=====================================
Several LED-Panels are available for small money.
Types have different sizes like P2, P3 ...P5 where number is size of pixels.
More interesting is ths nubmer of pixel, which differ from 32x32 up to 128x64
Espruino itself supports a JS based solution to control those boards.
Obviously, the larger the panel, the slower the refresh.

Idea of this class is, to support a solution, which is optimized for some boards.
Right now boards supported are ESP32 and Original Espruino Board.
Versions for MDBT42Q and ESP8266 are available, but not tested yet.
In addition some optimized functions graphics in zigzag mode are part of LedBoard

#### Usage ####
Since LedBoard is not part of original Espruino, you have to add following Files
- **jswrap_LedBoard.h** Header file
- **jswrap_LedBoard.c** Common part for all types of Espruino boards
- **LedBoardxxx.c** Optimized source for boards like ARM,ESP32,MDBT42Q,ESP8266 and last not least a default solution. At time writing this, only ARM and ESP32 is fully tested.

#### Limitations ####
- Only 8 colors are supported. Panels have only 3 color pins for RGB. To get more colors we would need something like PWM. This would take more CPU Power, and more RAM for graphics buffer. Thats not the focus of this simple solution.
- No hacks to avoid special effects like flickering. Original Espruino boards are more stable than ESP32. If somebody has an easy solution for that, please let me know.
- general solution is similiar for all boards. Optimization is done by accessing GPIO registers. Special HW like I2S for ESP32 is not supported.

####some basics ####
- Panels have 2 color pins for each color. pins with number 1 address pins in upper part of the panel. Pins with number 2 address pins in lower part.
- Columns are not adressed by pins. Color data for all pixel in a row are sent one after the other together with a clock signal
- after sending data of a row , adress of row is latched. Please have in mind that line in upper part and in lower part are sent ath the same time, see color pins.
- there are 3 pins for control, clock marks data for each column, latch is used to latch addresspins, enable is used to set row address 

#### LedBoard ####
- LedBoard is a library, and supports only one board at a time. This should not be a big problem, since each panel needs at least 13 pins. Last not least CPU of these simple boards are not designed for unlimited power.
- **init(datapins,addresspins,controlpins)**
datapins is an array of pins for R1,R2,G1,G2,B1,B2
addresspins is an array of pins for lines of data A,B,C,D(,E). Panels with 32 rows have 4 pins, panels with 64 rows need 5 pins. See some basics for more information.
controlpins is an array of pins enable, latch,clock. See some basics for more information.
- **send(graphics)** send graphics data to panel. Due to design of panels, data needs to be refreshed. Refreshrates of 50Hz give a stable image, even for 64x128 panel. For smaller panels like 32x64 refresh can go up to more than 100Hz. Arg for this function is a graphics object with initialized buffer.
- **postRefresh** is an event sent at the end of a send cycle. Can be helpful to synchronize changes in graphicsbuffer with refreshing panel.
- **activateEvent()** enables postRefresh event. After calling event, event is disabled. This is one more function to synchronize changes.
- **rotate(graphics,direction,frame)** rotates image data, optimized for zigzag graphice.
graphics object holding image data
direction of scrolling 1=left, 2 = right, 3 = up, 4 = down
frame definition  (x1,y1,x2,y2) if no data is given, whole image is used. One more definition is color. if no color is given, data is rotated, otherwise scrolldata is set to given color
- **reColor(graphics,newColors,frame)** 
graphics object holding image data.
Array of 8 colors. Color of pixel id index into this array. So each appearance of color black is replaced by first color in array, similiar happens for all other colors up to white pixels which are replaced by 8th item in new colors.
Frame defines area for this function
- **getFrame(graphics,frame)**
Returns an array with graphics data, defined by frame
- **setFrame(graphics,frame,frameData)**
copies frameData, usually created by getFrame, to graphics buffer
- if LEDBOARDDEBUG is set during make some more functions are available. Those are used for debugging. I used them during debugging with my analyzer, if you are interested, please take a closer look to c-source

#### example ####
- create graphics object based on size of panel
**grf = Graphics.createArrayBuffer(w,h,4,{interleavex:true});**
- init pins, example for Espruino Board
**LedBoard.init([B3,B5,C11,C10,B4,B6],[B7,C9,B8,C8,B12],[C12,C6,B9]);**
- create your imag, using graphics functions, or load file from using Storage or fs, (or something else ?)
- refresh data
**setInterval(function(){LedBoard.send(grf);},20);**
- for some more ideas see example in github



