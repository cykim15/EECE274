#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"


#include <math.h>
#include <stdio.h>
#include <float.h>


void mirror_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void grayScale_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void sobelFiltering_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out);

int main()
{
 
	int height;
	int width;
	int channel;

	char command;

 	printf("Take a picture? (y/n)\n");
 	scanf("%c", &command);

 	if(command == 'n')
 		exit(1);


 	printf("Cheeze !\r\n");
        system("libcamera-still --width 640 --height 480 -t 10 -o image.bmp");
 	
 	unsigned char* imgIn = stbi_load("image.bmp", &width, &height, &channel, 3);

	unsigned char* imgOut_mirror = (unsigned char*) malloc (sizeof(unsigned char)*3*640*480);
	unsigned char* imgOut_grayScale = (unsigned char*) malloc (sizeof(unsigned char)*3*640*480);
	unsigned char* imgOut_sobelFiltering = (unsigned char*) malloc (sizeof(unsigned char)*3*640*480);
 	
	mirror_transform(imgIn, height, width, channel, imgOut_mirror);
	grayScale_transform(imgIn, height, width, channel, imgOut_grayScale);
	sobelFiltering_transform(imgOut_grayScale, height, width, channel, imgOut_sobelFiltering);


	stbi_write_bmp("image_mirror.bmp", width, height, channel, imgOut_mirror);
	stbi_write_bmp("image_grayScale.bmp", width, height, channel, imgOut_grayScale);
	stbi_write_bmp("image_sobelFiltering.bmp", width, height, channel, imgOut_sobelFiltering);
	
	stbi_image_free(imgIn);
	free(imgOut_mirror);
	free(imgOut_grayScale);
	free(imgOut_sobelFiltering);
 

	return 0;
}

void mirror_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
    int i = 0; // in의 인덱스
    int h, w; int k = 0;
	for(h=1; h<=height; h++) { // 세로 반복
		for(w=1; w<=width*channel; w++){ // 가로 반복	    
			out[channel * (2*width*h - width -1) + 2*k - i] = in[i];
			i++;
			k++;
			if(k == channel) {
				k = 0;
			}
		}
	}
	return;
}

void grayScale_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
    int i = 0;// in의 인덱스
	int sum = 0;
	int h, w, a; int k = 0;
	for(h=1; h<=height; h++) { // 세로 반복
		for(w=1; w<=width*channel; w++){ // 가로 반복
			sum += in[i];
		    if(k == channel-1) {
				for(a=0; a<channel; a++) {
					out[i-a] = sum/channel;
				}
			    sum=0; k=0;
			}
			else {
				k++;
			}
			i++;
		}
	}
	return;
}

void sobelFiltering_transform (unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
	int i = 0;// in의 인덱스
	int h, w, n, k, o;
	float x_filter, y_filter;
	 
	for(n=0; n<channel*width; n++) { // zero padding: 위 아래
		out[n] = 0; out[n+channel*width*(height-1)] = 0;
	}
	for(n=0; n<height; n++) { // zero padding: 양 옆
		for(k=0; k<channel; k++) {
			out[k + channel*width*n] = 0; out[k + channel*(width-1) + channel * width *n] = 0;
		} 
	}
	
	
	o = channel * width + channel; // padding으로 인한 out의 시작 index
	
	for(h=1; h<=height-4; h++) { // 세로 반복
		for(w=1; w<=width-2; w++){ // 가로 반복
		    x_filter = abs(in[i]*(-1) + in[i+2*channel]*(1) + in[i+channel*width]*(-2) + in[i+2*channel+channel*width]*(2) + in[i+2*channel*width]*(-1) + in[i+2*channel+2*channel*width]*(1));
		    y_filter = abs(in[i]*(1) + in[i+channel]*(2) + in[i+2*channel]*(1) + in[i+2*channel*width]*(-1) + in[i+channel+2*channel*width]*(-2) + in[i+2*channel+2*channel*width]*(-1));
		    for(k=0; k<channel; k++) {
		    	out[o+k] = x_filter + y_filter;
			}
			o += channel;
			i += channel;		
		}
		o += 2*channel;
		i += 2*channel;
	}
	return;
}
