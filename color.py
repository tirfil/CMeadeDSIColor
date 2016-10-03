import time
import sys
import os
import shutil
import re

rx_base = re.compile("\/([0-9_]*)\.fits")
rx_dir = re.compile("raw.(.*)$")

path=r"~/Documents/DSI/CMeadeDSIColor/"

#Create repositories
if not os.path.isdir("work"):
	os.mkdir("work")
	
if not (os.path.isdir("results/raw/image") and os.path.isdir("results/raw/dark") 
	and os.path.isdir("results/raw/flat") and os.path.isdir("results/raw/offset")):
		print("Missing directori(y/es)");
		exit(0);
		
if not os.path.isdir("results/color"):
	os.mkdir("results/color")
			
for d in ["image","dark","flat","offset"]:
	d2 = "results/color/" + d
	if not os.path.isdir(d2):
		os.mkdir(d2)
	for c in ["red","green","blue"]:
		d3 = d2 + "/" + c
		if not os.path.isdir(d3):
			os.mkdir(d3)

os.chdir("work")
	
for root, dirs, files in os.walk("../results/raw"):
	for name in files:
		source = root + "/" + name
		md = rx_base.search(source)
		base = ""
		directory = ""
		if md:
			base = md.group(1)
		md = rx_dir.search(root)
		if md:
			directory = md.group(1)	
		if base == "" or directory == "":
			print("Unknown file: %s\n" % source)
			exit(0)
			
		source = "../results/raw/" + directory + "/" + base + ".fits"
		cmd = path + "cmyg2rgb " + source
		#print cmd
		os.system(cmd)
		destination = "../results/color/" + directory + "/red/" + base + "_red.fits"
		shutil.move("red.fits",destination)
		destination = "../results/color/" + directory + "/green/" + base + "_green.fits"
		shutil.move("green.fits",destination)
		destination = "../results/color/" + directory + "/blue/" + base + "_blue.fits"
		shutil.move("blue.fits",destination)
		os.remove("luminance.fits")
