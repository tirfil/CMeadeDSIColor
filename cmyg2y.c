#include <string.h>
#include <stdio.h>
#include "fitsio.h"

int main(int argc, char *argv[])
{
    fitsfile *fptr;    
    fitsfile *fptrout;
    int status = 0;
    unsigned int bitpix, naxis;
    long naxes[2];
    long nelements;
    unsigned short *image;
    unsigned short *imageout;
    int anynul;
    
    int x,y;
    unsigned int yy;
    int index;
    
    if(argc != 3) {
		return 0;
	}
	
	if (!fits_open_file(&fptr, argv[1], READONLY, &status))
    {
		if (!fits_get_img_param(fptr, 2, &bitpix, &naxis, naxes, &status) )
        {
			printf("bitpix=%d naxes=%d width=%ld heigth=%ld\n",bitpix,naxis,naxes[0],naxes[1]);
			nelements = naxes[0]*naxes[1];
			image = malloc(sizeof(short)*nelements);
			
			if (bitpix == 16){
				fits_read_img(fptr,TUSHORT,1,nelements,NULL,image, &anynul, &status);

				nelements = (naxes[0]-1)*(naxes[1]);
				imageout = malloc(sizeof(short)*nelements);
				index = 0;
				for(y=0;y<naxes[1];y++)
					for(x=1;x<naxes[0];x++) {
						yy = image[x+y*naxes[0]] + image[x-1+y*naxes[0]];
						imageout[index++] = (unsigned short)(yy/2);
					}
				
				naxes[0]--;
				naxes[1]--;
				
				fits_create_file(&fptrout,argv[2], &status);
				fits_create_img(fptrout, SHORT_IMG, naxis, naxes, &status);
				fits_write_img(fptrout, TUSHORT, 1, nelements, imageout, &status);
				fits_close_file(fptrout, &status);
			}
			free(image);
		}
	}
	fits_close_file(fptr, &status);
	if (status) fits_report_error(stderr, status);
    return(status);
}
