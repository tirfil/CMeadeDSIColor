import time
import sys
import os

path=r"~/Documents/DSI/CMeadeDSIColor/"

duration = sys.argv[1] # 1/10000 sec
number = int(sys.argv[2])

for i in range(number):
	now = time.localtime()
	base =time.strftime("%Y%m%d_%H%M%S",now)
	os.mkdir(base)
	os.chdir(base)
	cmd = path+"MeadeDSIColor -e " + duration + " -g 10 -o 0 -f"
	os.system(cmd)
	cmd = path+"cmyg2rgb cmyg.fits"
	os.system(cmd)
	os.chdir("..")
	time.sleep(1)
	

