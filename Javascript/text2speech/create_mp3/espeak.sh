#espeak --voices
espeak -v german -w $2.wav $1 
ffmpeg -i $2.wav mp3_de/$2.mp3
rm $2.wav
