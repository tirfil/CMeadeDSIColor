import time
import sys
import os
import shutil

path=r"~/Documents/DSI/CMeadeDSIColor/"

directory = "flat"

duration = sys.argv[1] # 1/10000 sec
number = int(sys.argv[2])

# Create repositories
if not os.path.isdir("work"):
	os.mkdir("work")

if not os.path.isdir("results"):
	os.mkdir("results")
	
if not os.path.isdir("results/raw"):
	os.mkdir("results/raw")
	
if not os.path.isdir("results/raw/"+directory):
	os.mkdir("results/raw/"+directory)
	
os.chdir("work")

for i in range(number):
	now = time.localtime()
	base =time.strftime("%Y%m%d_%H%M%S",now)
	cmd = path+"MeadeDSIColor -e " + duration + " -g 10 -o 0 -f"
	os.system(cmd)
	#os.system("touch cmyg.fits")
	destination = "../results/raw/" + directory + "/" + base + "_" + str(i) +".fits"
	shutil.move("cmyg.fits",destination)
	
	
