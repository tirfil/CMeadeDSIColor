import time
import sys
import os
import shutil

path=r"~/Documents/DSI/CMeadeDSIColor/"

if not os.path.isdir("save"):
	os.mkdir("save")

if not os.path.isdir("work"):
	os.mkdir("work")
os.chdir("work")

if os.path.isfile("cmyg.fits"):
	os.remove("cmyg.fits")
	
if os.path.isfile("color.png"):
	os.remove("color.png")
	
now = time.localtime()
base = time.strftime("%Y%m%d_%H%M%S",now)

duration = sys.argv[1] # 1/10000 sec

cmd = path+"MeadeDSIColor -e " + duration + " -g 10 -o 100 -f"
os.system(cmd)
cmd = path+"cmyg2png cmyg.fits"
os.system(cmd)

destination = "../save/" + base +".fits"
shutil.move("cmyg.fits",destination)

cmd = "eog color.png"
ret = os.system(cmd)

os.chdir("..")

