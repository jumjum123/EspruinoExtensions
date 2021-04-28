import pyttsx3
engine = pyttsx3.init()
print(engine)
voices = engine.getProperty('voices')
for voice in voices:
	print(voice.name,voice.gender)
engine.save_to_file('Hello World' , 'test.mp3')
engine.runAndWait()
