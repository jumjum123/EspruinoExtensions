Text 2 Speech (very simple)
=====================================
After reading something about speech in a hacker magazin, I was a kind of disappointed. Everything they wrote about needed RPI or was designed for distinct board.
Therefore I decided to search for a simple and cheap solution.
At the end it became a solution of a small MP3 player module with serial interface.
MP3 Player is DFPlayer Mini MP3 Player Module, which you can find for less than an €. Loudspeaker is directly connected and is less than an € too. Add an SD-Card for mp3 files, that's it.
Idea behind this project is to have spoken words in mp3 files. Start was with numbers, I created 99 files for number 1 to 99 and stored them on SD-Card.
The mp3-player has three options for control
- **Binary pins** for simple handling like next/previous file and loudness
- **Analog pins** some more commands, you need some resistors to create different levels
- **Serial pins** with a lot of options for commands

First 2 options are nice for testing, but for more control, decisions was easy. Serial pins are taken for that.
I like Espruino for ease of usage for small projects like this. So a simple javascript solution was born (see text2speech.js)

#### Javascript ####
My favourite design is Q&D (quick and dirty) and so I did.
For use, you only have to create an object handing over a serial port.
##### Functions #####
For all commands you will find some examples in source file.
- **create object** *var txt2speech = new text2SpeechCmd(serial2)* 
- **volume** *txt2speech.volume(volume,callback)* volume is from 0 to 30
- **play** *txt2speech.play(dir,file,callback)* dir is directory from 1 to 12, file is filenumber from 0001 to 2999
- **playMulti** *txt2speech.playMulti(files, timeBetween,callback)* files is an array of mp3 files which are played one after the other. timeBetween defines delay before next mp3 file is called.

#### organize mp3 files ####
The board supports several options to store and play mp3 files. Name of files follows simple guidelines. Naming is a number from like 0001,0002,etc with extension mp3. Max number depends on folder.
There are 3 types of folders
- **mp3** general folder for 250 files from 0001.mp3 to 0250.mp3
- **number** number is from 1 to 12, and files in this folder are from 0001 to 2999
- **ad** one more special folder for advertising. This is supported for playing files, not for advertising

Since there is no restriction for size of files, we could store whole sentences in files. Other option is to store words only and play them one after the other. For example if you want to speak a voltage value, you play 4 files for **2** , **hundred** , **40** , **volt**
But how do you get mp3 files. There are several options. You could record your own voice. This is by far the most personal way. And it takes a lot of time. Imagine to talk all numbers from 0 to 99.
There are some tools available online to convert text to mp3. This also takes some time since you have to convert for example 99 numbers. If you are familiar with debug console of your browser, you could create a hack to create lots of files. But this is not correct, as long as you don't get ok from service owner.
Googling around I found some other tools which can be helpful. In folder create_mp3 are some low level solutions.