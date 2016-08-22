Mapped Files, simple file handling in flash
===========================================
Sometimes, you would like to have a kind of files in your application. 
Examples are small html-files, or Javascript modules as mentioned by @tve in the ++__[Espruino forum](http://forum.espruino.com/conversations/290975/#comment13146088)__++. @tve gives a good starting point. One restriction is that you use a full page for one module, independing from size of your module.
This module is an option to handle as much strings as possible, based on free flash memory. There is a simple kind of directory which supports addressing a string by or by number.
Directory always is at begin of Flash Page and looks not easy to create. You could use a __++[service to create and download your own file](http://www.jumware.eu:89/file?fileName=createESP8266Binary)++__ and copy it to flash with tool of your choice.
General description of directory
--------------------------------
0x00-0x03 number of files
0x04-0x05 length of first file
0x06-0x07 start of first file
0x08-0x0a length of filename
0x09-0x13 filename, filled up with 0
0x14-0x15 length of 2nd file
0x16-0x17 start of 2nd file
.........

####Flash####

- **mappedString(Start of page,Filename or Id)** 
**returns** Mapped String
- **mappedDir(Start of page where strings are stored)** 
**returns** StringArray of files
- **mappedArray(Start of page where strings are stored,Filename)** 
**returns** A Uint8Array

