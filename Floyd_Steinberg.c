#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
int find_closest_palette_color(int oldpixel){
    //converts from 0-255 grayscale to 0-1, black-white
    return round(((double) oldpixel) / 255);
}

uint8_t** readImage(char* inputFile, int x_size, int y_size){
    uint8_t** pixels = (uint8_t**)malloc(y_size * sizeof(uint8_t*));

    FILE *f = fopen(inputFile, "r");
    for (int y = 0; y < y_size; y++) {
        pixels[y] = (uint8_t*)malloc(x_size * sizeof(uint8_t));
        for (int x = 0; x < x_size; x++) {
            int pixel;
            fscanf(f, "%d", &pixel);
            pixels[y][x] = pixel;
        }
    }
    fclose(f);

    return pixels;
}

void floydSteinbergDither(uint8_t** pixels, int x_size, int y_size){
    for (int y = 1; y < y_size; y++) {
        for (int x = 0; x < x_size; x++) {
            uint8_t oldpixel = pixels[y][x];
            uint8_t newpixel = find_closest_palette_color(oldpixel);
            uint8_t quant_error = oldpixel - newpixel;
            pixels[y][x] = newpixel;

            if(x < x_size-1){
                pixels[y][x + 1] += quant_error * 7 / 16;
            }
    
            if(x > 0 && y < y_size - 1){
                pixels[y + 1][x - 1] += quant_error * 3 / 16;
            }
    
            if(y < y_size - 1){
                pixels[y + 1][x] += quant_error * 5 / 16;
            }
    
            if(x < x_size - 1 && y < y_size - 1){
                pixels[y + 1][x + 1] += quant_error * 1 / 16;
            }
        }
    }
}

void interpolateImage(uint8_t** pixels, int x_size, int y_size){
    for (int y = 0; y < y_size; y++) {
        for (int x = 0; x < x_size; x++) {
            pixels[y][x] += 255;
        }
    }
}

void writeImage(uint8_t** pixels, int x_size, int y_size, char* outputFile){
    FILE *f5 = fopen(outputFile, "w");
    fprintf(f5, "P2\n%d %d\n", x_size, y_size); //P2 format, dimensions
    fprintf(f5, "255\n"); //255 bit depth
    for (int y = 0; y < y_size; y++) {
        for (int x = 0; x < x_size; x++) {
            fprintf(f5, "%d\n", pixels[y][x]);
        }
    }
    fclose(f5);
}

int dither(char* inputFile, char* outputFile, int x_size, int y_size, int passes) {
    uint8_t** pixels = readImage(inputFile, x_size, y_size);
    for(int p = 0; p < passes; p++){
        floydSteinbergDither(pixels, x_size, y_size);
        interpolateImage(pixels, x_size, y_size);
    }
    writeImage(pixels, x_size, y_size, outputFile);

    return 0;
}