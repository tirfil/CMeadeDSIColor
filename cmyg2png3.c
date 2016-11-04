#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "fitsio.h"
#define PNG_DEBUG 3
#include <png.h>

// 
// transform cmyg mosaic from SONY ICS404AK sensor into RGB component
// use fits format as input and output
// 
// adapted to MeadeDSIColor software
// outputs are red.fits green.fits blue.fits and luminance.fits
//

// Use color coding as described in the ICX404AK specification

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

void change_endianess(unsigned short* tab , int size)
{
	int i;
	for(i=0;i<size;i++)
	{
		tab[i] = (tab[i] & 0xff00) >> 8 | (tab[i] & 0x00ff) << 8;
	}
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
    unsigned short *imageout;
    int anynul;
    
    int xoffset = 23;
    int yoffset = 13+1;
    
    unsigned short p00,p10,p11,p01;
    int cyan,yellow,magenta,cgreen;
    int mode;
    
    int x,y;
    int index;
    
    int i;
    
    int Y, CR, CBN;
    
    unsigned short maxvalue=0;
	unsigned short minvalue=65535;
	unsigned short value;
	int factor,offset;
    
    
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
				red = malloc(sizeof(unsigned short)*nelements);
				green = malloc(sizeof(unsigned short)*nelements);
				blue = malloc(sizeof(unsigned short)*nelements);
				luminance = malloc(sizeof(unsigned short)*nelements);
				imageout = malloc(sizeof(unsigned short)*nelements*3);
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
						
						// as described in ICX404AK specification p9/18
						Y = ((cgreen + cyan) + (magenta + yellow))/2;
						// chroma CR = R-Y
						CR = (magenta + yellow)-(cgreen + cyan);
						// chroma CB = -(B-Y) ; CBN = -CB
						CBN = (magenta + cyan)-(cgreen + yellow);
						
						red[index] =   bounds(0,65535,(CR + Y));
						green[index] = bounds(0,65535,cgreen);
						blue[index] =  bounds(0,65535,(CBN + Y));
						luminance[index] = bounds(0,65535,Y);
						
						imageout[3*index]   = red[index];
						imageout[3*index+1] = green[index];
						imageout[3*index+2] = blue[index];
						index++;
						
					}
					
				printf("underflow = %d \n",underflow);
				printf("overflow = %d \n",overflow);
				
				naxes[0]--; // one row and columm less
				naxes[1]--;
				
				//Improve luminosity
				
				for(y=0;y<naxes[1];y++)
					for(x=0;x<naxes[0]*3;x++){
						value = imageout[x+3*naxes[0]*y];
						if (maxvalue < value) maxvalue = value;
						if (minvalue > value) minvalue = value;
					}
					
				printf("max=%d\n",maxvalue);
				printf("min=%d\n",minvalue);	
			

				factor = 65535/(maxvalue-minvalue);
				offset = minvalue;
				printf("factor=%d\n",factor);
				printf("offset=%d\n",offset);
				
				for(y=0;y<naxes[1];y++)
					for(x=0;x<naxes[0]*3;x++){
						value = imageout[x+3*naxes[0]*y];
						value = (value-offset)*factor;
						imageout[x+3*naxes[0]*y] = value;
					}

				int width = naxes[0];
				int height = naxes[1];
				
				FILE *fp = fopen("color.png", "wb");
				if (!fp) {
					fprintf(stderr,"Impossible to open for writing\n");
					return -1;
				}
				
				png_structp  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
				if (!png_ptr) {
					fprintf(stderr,"Error: png_create_write_struct\n");
					fclose(fp);
					return -1;
				}
				
				png_infop  info_ptr = png_create_info_struct(png_ptr);
				if (!info_ptr) {
					fprintf(stderr,"Error: png_create_info_struct\n");
					png_destroy_write_struct(&png_ptr,(png_infopp)NULL);
					fclose(fp);
					return -1;
				}
				
				if (setjmp(png_ptr->jmpbuf)) {
					fprintf(stderr,"Error: setjmp png_create_info_struct\n");	
					png_destroy_write_struct(&png_ptr, &info_ptr);
					fclose(fp);
					return -1;
				}
				
				png_init_io(png_ptr, fp);

				if (setjmp(png_ptr->jmpbuf)) {
					fprintf(stderr,"Error: setjmp png_init_io\n");	
					png_destroy_write_struct(&png_ptr, &info_ptr);
					fclose(fp);
					return -1;
				}
				
				png_byte color_type = PNG_COLOR_TYPE_RGB;
				png_byte bit_depth = 16;
				
				png_set_IHDR(png_ptr, info_ptr, width, height,
                     bit_depth, color_type, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
                     
                png_write_info(png_ptr, info_ptr);
                
                if (setjmp(png_ptr->jmpbuf)) {
					fprintf(stderr,"Error: setjmp png_write_info\n");	
					png_destroy_write_struct(&png_ptr, &info_ptr);
					fclose(fp);
					return -1;
				}
				
				// transform unsigned int -> unsigned char
				// big endian !!!
				unsigned char *buf = NULL;
				buf = (unsigned char *)malloc(sizeof(char)*width*height*6);
				
				change_endianess(imageout,width*height*3);				
				memcpy(buf, imageout, width*height*6);
				
				png_bytepp rows = (png_bytepp)png_malloc(png_ptr,sizeof(png_bytep)*height);
				for (y = 0; y < height; ++y) {
					rows[y] = (png_bytep)(buf + (height -y) * width * 6);
				}
				
                
                png_write_image(png_ptr, rows);
                
                
                if (setjmp(png_ptr->jmpbuf)) {
					fprintf(stderr,"Error: setjmp png_write_image\n");	
					png_destroy_write_struct(&png_ptr, &info_ptr);
					fclose(fp);
					return -1;
				}
				
				png_write_end(png_ptr, NULL);
				
				png_destroy_write_struct(&png_ptr, &info_ptr);
				
				free(buf);
				fclose(fp);
				
	
			}
			free(image);
			free(red); free(green); free(blue); free(imageout);
		}
	}
	fits_close_file(fptr, &status);
	if (status) fits_report_error(stderr, status);
    return(status);
}
