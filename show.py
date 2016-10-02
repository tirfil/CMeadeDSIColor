import time
import sys
import os

path=r"~/Documents/DSI/CMeadeDSIColor/"

if not os.path.isdir("work"):
	os.mkdir("work")
os.chdir("work")

duration = sys.argv[1] # 1/10000 sec

cmd = path+"MeadeDSIColor -e " + duration + " -g 10 -o 0 -f"
os.system(cmd)
cmd = path+"cmyg2tiff cmyg.fits"
os.system(cmd)
cmd = "eog color.tif"
ret = os.system(cmd)
os.remove("cmyg.fits")
os.remove("color.tif")

os.chdir("..")

