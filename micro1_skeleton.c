#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"


#include <math.h>
#include <stdio.h>
#include <float.h>


void mirror_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void grayScale_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out);
void sobelFiltering_transform(unsigned char* zp, unsigned char* in, int const height, int const width, int const channel, unsigned char* out);

int main()
{

    int height;
    int width;
    int channel;

    char command;

    printf("Take a picture? (y/n)\n");
    scanf("%c", &command);

    if (command == 'n')
        exit(1);


    printf("Cheeze !\r\n");
    system("libcamera-still -width 640 -height 480 -o image.bmp");


    unsigned char* imgIn = stbi_load("image.bmp", &width, &height, &channel, 3);


    unsigned char* imgOut_mirror = (unsigned char*)malloc(sizeof(unsigned char) * 3 * 640 * 480);
    unsigned char* imgOut_grayScale = (unsigned char*)malloc(sizeof(unsigned char) * 3 * 640 * 480);
    unsigned char* imgOut_sobelFiltering = (unsigned char*)malloc(sizeof(unsigned char) * 3 * 640 * 480);

    //zero_padding
    unsigned char* zero_padding = (unsigned char*)malloc(sizeof(unsigned char) * 3 * 642 * 482);

    mirror_transform(imgIn, height, width, channel, imgOut_mirror);
    grayScale_transform(imgIn, height, width, channel, imgOut_grayScale);
    sobelFiltering_transform(zero_padding, imgOut_grayScale, height, width, channel, imgOut_sobelFiltering);


    stbi_write_bmp("image_mirror.bmp", width, height, channel, imgOut_mirror);
    stbi_write_bmp("image_grayScale.bmp", width, height, channel, imgOut_grayScale);
    stbi_write_bmp("image_sobelFiltering.bmp", width, height, channel, imgOut_sobelFiltering);

    stbi_image_free(imgIn);
    free(imgOut_mirror);
    free(imgOut_grayScale);
    free(imgOut_sobelFiltering);


    return 0;
}

void mirror_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            for (int c = 0; c < channel; c++)
            {
                int src_index = (y * width + x) * channel + c;
                int dst_index = (y * width + (width - 1 - x)) * channel + c;
                out[dst_index] = in[src_index];
            }
        }
    }

}

void grayScale_transform(unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int sum = 0;
            for (int c = 0; c < channel; c++)
            {
                int index = (y * width + x) * channel + c;
                sum += in[index];
            }
            int avg = sum / channel;
            for (int c = 0; c < channel; c++) {
                int index = (y * width + x) * channel + c;
                out[index] = avg;
            }
        }
    }
}

void sobelFiltering_transform(unsigned char* zp, unsigned char* in, int const height, int const width, int const channel, unsigned char* out) {

    for (int y = 0; y < height + 2; y++) {
        for (int x = 0; x < width + 2; x++) {
            for (int c = 0; c < channel; c++) {
                int index = (y * width + x) * channel + c;
                if (x<1 || y<1 || x > width || y > height) {
                    zp[index] = 0;
                }
                else {
                    zp[index] = in[index];
                }
            }
        }
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            for (int c = 0; c < channel; c++) {
                int id0 = (y * width + x) * channel + c;
                int id1 = (y * width + (x + 1)) * channel + c;
                int id2 = (y * width + (x + 2)) * channel + c;
                int id3 = ((y + 1) * width + x) * channel + c;
                int id4 = ((y + 1) * width + (x + 2)) * channel + c;
                int id5 = ((y + 2) * width + x) * channel + c;
                int id6 = ((y + 2) * width + (x + 1)) * channel + c;
                int id7 = ((y + 2) * width + (x + 2)) * channel + c;

                int abs_x = abs(zp[id0] * (-1) + zp[id2] * (1) + zp[id3] * (-2) + zp[id4] * (2) + zp[id5] * (-1) + zp[id7] * (1));
                int abs_y = abs(zp[id0] * (1) + zp[id1] * (2) + zp[id2] * (1) + zp[id5] * (-1) + zp[id6] * (-2) + zp[id7] * (-1));

                out[id0] = abs_x + abs_y;
            }
        }
    }

}