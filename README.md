# CMeadeDSIColor

Meade DSI Color CCD capture software.

Firmware must be load bofore into CCD: fxload -I firmware.fw -D /dev/bus/device/<#bus>/<#device> or via UDEV

Output raw image with CMYG mosaic (SONY ICX404AK sensor)


*cmyg2rgb*

Transform cmyg mosaic from SONY ICS404AK sensor into RGB components.

Use fits as input and output format.

Outputs are red.fits green.fits blue.fits and luminance.fits
