
import sys
import os
import shutil
import re

exepath=r"~/Documents/DSI/CMeadeDSIColor/"

path = sys.argv[1]

cmd = exepath + "cmyg2png "

for root, dirs, files in os.walk(path):
	for name in files:
		source = root + os.sep + name
		base,ext = os.path.splitext(name)
		if ext == ".fits":
			cmd = exepath + "cmyg2png " + source
			os.system(cmd)
			shutil.move("color.png",base+".png")
