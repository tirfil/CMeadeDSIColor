import os
import sys
import time
import signal

path=r"~/Documents/DSI/CMeadeDSIColor/"
path2=r"~/Documents/DSI/AstroPhotoProcessing/Focus/"

if not os.path.isdir("work"):
	os.mkdir("work")
os.chdir("work")

for i in range(50):
	cmd = path+"MeadeDSIColor -e 100 -g 10 -o 0 -f"
	os.system(cmd)
	cmd = path2+"starfocus cmyg.fits 5"
	os.system(cmd)
	time.sleep(1)
	os.remove("cmyg.fits")
	
	
