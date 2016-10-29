import os
import sys
import time
import signal

path=r"~/Documents/DSI/CMeadeDSIColor/"

if not os.path.isdir("work"):
	os.mkdir("work")
os.chdir("work")

for i in range(10):
	cmd = path+"MeadeDSIColor -e 100 -g 10 -o 0 -f"
	os.system(cmd)
	cmd = path+"cmyg2png cmyg.fits"
	os.system(cmd)
	pid = os.fork()
	if not pid:
		cmd = "eog color.png"
		ret = os.system(cmd)
		exit(0)
	else:
		time.sleep(5)
		print "kill",pid
		os.kill(pid+2,signal.SIGTERM)
		os.wait()
	os.remove("cmyg.fits")
	os.remove("color.tif")
	
	
