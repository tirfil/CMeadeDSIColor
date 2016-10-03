# CMeadeDSIColor

Meade DSI Color CCD capture software.

Beware:
Firmware must be load before into CCD: fxload -t fx2 -I firmware.fw -D /dev/bus/usb/<#bus>/<#device> or via UDEV rules: 
see https://github.com/MParygin/l.dsi2pro.fw or meade-deepskyimager.hex on https://github.com/indilib/indi

Output raw image with CMYG mosaic (SONY ICX404AK sensor)


*cmyg2rgb*

Transform cmyg mosaic from SONY ICS404AK sensor into RGB components.

Use fits as input and output format.

Outputs are red.fits green.fits blue.fits and luminance.fits

*cmyg2tiff*

Transform cmyg mosaic from SONY ICS404AK sensor into TIFF format image.

*focus.py < exposure >*

Python script for CCD focus setting. (use eog (eye of gnome) viewer)

*show.py < exposure >*

Python script to display CCD current capture. (use eog viewer)

*multi.py < exposure > < number >*

Python script to capture sequence of CCD images.
