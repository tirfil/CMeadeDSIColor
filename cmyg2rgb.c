#include <string.h>
#include <stdio.h>
#include "fitsio.h"

// 
// transform cmyg mosaic from SONY ICS404AK sensor into RGB component
// use fits format as input and output
// 
// adapted to MeadeDSIColor software
// outputs are red.fits green.fits blue.fits and luminance.fits
//

int underflow = 0;
int overflow = 0;

unsigned short bounds(unsigned short min, unsigned short max, int value)
{
	if (value < min){
		underflow++;
		return min;
	}
	
	if (value > max){
		overflow++;
		return max;
	}
	
	return (unsigned short)value;
}

int main(int argc, char *argv[])
{
    fitsfile *fptr;    
    fitsfile *fptrout;
    int status = 0;
    unsigned int bitpix, naxis;
    long naxes[2];
    long nelements;
    unsigned short *image;
    unsigned short *red;
    unsigned short *green;
    unsigned short *blue;
    unsigned short *luminance;
    int anynul;
    
    int xoffset = 23;
    int yoffset = 13+1;
    
    unsigned short p00,p10,p11,p01;
    int cyan,yellow,magenta,cgreen;
    int mode;
    
    int x,y;
    int index;
    
    if (argc == 3) {
		mode = atoi(argv[2]);
		yoffset = mode/2;
		xoffset = mode%2;
	} else 
		if(argc != 2){
			printf("Usage:\n%s <cmyg.fits> [mosaic position]\n",argv[0]);
			//printf("exiting %d\n",argc);
			return 0;
		}
	
	if (!fits_open_file(&fptr, argv[1], READONLY, &status))
    {
		if (!fits_get_img_param(fptr, 2, &bitpix, &naxis, naxes, &status) )
        {
			//printf("bitpix=%d naxes=%d width=%ld heigth=%ld\n",bitpix,naxis,naxes[0],naxes[1]);
			nelements = naxes[0]*naxes[1];
			image = malloc(sizeof(short)*nelements);
			
			if (bitpix == 16){
				fits_read_img(fptr,TUSHORT,1,nelements,NULL,image, &anynul, &status);

				nelements = (naxes[0]-1)*(naxes[1]-1);
				red = malloc(sizeof(short)*nelements);
				green = malloc(sizeof(short)*nelements);
				blue = malloc(sizeof(short)*nelements);
				luminance = malloc(sizeof(short)*nelements);
				index = 0;
				
				for(y=1;y<naxes[1];y++)
					for(x=1;x<naxes[0];x++) {
						p00 = image[x+y*naxes[0]];
						p10 = image[x-1+y*naxes[0]];
						p01 = image[x+(y-1)*naxes[0]];
						p11 = image[x-1+(y-1)*naxes[0]];
						mode = ((yoffset+y)%4)*2+((xoffset+x)%2);
						// conform to ICX404AK CMYG mosaic
						switch(mode){
							case 0:
								cgreen = p11;
								yellow = p10;
								cyan = p00;
								magenta=p01;
								break;
							case 1:
								cgreen = p01;
								yellow = p00;
								cyan = p10;
								magenta= p11;
								break;
							case 2:
								cgreen = p00;
								yellow = p11;
								cyan = p01;
								magenta=p10;
								break;
							case 3:
								cgreen = p10;
								yellow = p01;
								cyan = p11;
								magenta=p00;
								break;
							case 4:
								cgreen = p01;
								yellow = p10;
								cyan = p00;
								magenta=p11;
								break;
							case 5:
								cgreen = p11;
								yellow = p00;
								cyan = p10;
								magenta=p01;
								break;
							case 6:
								cgreen = p10;
								yellow = p11;
								cyan = p01;
								magenta=p00;
								break;
							case 7:
								cgreen = p00;
								yellow = p01;
								cyan = p11;
								magenta=p10;
								break;
							default:
								printf("ERROR: unknown mode %d\n",mode);
								cgreen = 0;
								yellow = 0;
								cyan = 0;
								magenta=0;
						} 
						
						//red[index] =   bounds(0,65535,(magenta + yellow - cyan)/2);
						//green[index] = bounds(0,65535,(cyan + yellow - magenta)/2);
						//green[index] = bounds(0,65535,cgreen);
						//blue[index] =  bounds(0,65535,(magenta + cyan - yellow)/2);

						// change color algo
						red[index] =   bounds(0,65535,(magenta + yellow)/2);
						green[index] = bounds(0,65535,(cyan + yellow)/2);
						//green[index] = bounds(0,65535,cgreen);
						blue[index] =  bounds(0,65535,(magenta + cyan)/2);
						luminance[index] = bounds(0,65535,(magenta + cyan + yellow)/3);
						index++;
						
					}
					
				printf("underflow = %d \n",underflow);
				printf("overflow = %d \n",overflow);
				
				naxes[0]--; // one row and columm less
				naxes[1]--;
				
				fits_create_file(&fptrout,"red.fits", &status);
				fits_create_img(fptrout, USHORT_IMG, naxis, naxes, &status);
				fits_write_img(fptrout, TUSHORT, 1, nelements, red, &status);
				fits_close_file(fptrout, &status);
				fits_create_file(&fptrout,"green.fits", &status);
				fits_create_img(fptrout, USHORT_IMG, naxis, naxes, &status);
				fits_write_img(fptrout, TUSHORT, 1, nelements, green, &status);
				fits_close_file(fptrout, &status);
				fits_create_file(&fptrout,"blue.fits", &status);
				fits_create_img(fptrout, USHORT_IMG, naxis, naxes, &status);
				fits_write_img(fptrout, TUSHORT, 1, nelements, blue, &status);
				fits_close_file(fptrout, &status);
				fits_create_file(&fptrout,"luminance.fits", &status);
				fits_create_img(fptrout, USHORT_IMG, naxis, naxes, &status);
				fits_write_img(fptrout, TUSHORT, 1, nelements, luminance, &status);
				fits_close_file(fptrout, &status);
			}
			free(image);
			free(red); free(green); free(blue);
		}
	}
	fits_close_file(fptr, &status);
	if (status) fits_report_error(stderr, status);
    return(status);
}
