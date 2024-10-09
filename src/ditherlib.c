#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

enum DitherAlgorithm {
    FLOYD_STEINBERG,
    ATKINSON
};


int find_closest_palette_color(int oldpixel){
    //converts from 0-255 grayscale to 0-1, black-white
    return round(((double) oldpixel) / 255);
}

void floydSteinbergDither(uint8_t** pixels, int y, int x, int y_size, int x_size, uint8_t quant_error){
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

void atkinsonDither(uint8_t** pixels, int y, int x, int y_size, int x_size, uint8_t quant_error){
    // Right neighbours
    if(x < x_size-1){
        pixels[y][x + 1] += quant_error * 1 / 8;
    }
    if(x < x_size-2){
        pixels[y][x + 2] += quant_error * 1 / 8;
    }

    // Bottom Right neighbours
    if(x < x_size-1 && y < y_size - 1){
        pixels[y + 1][x + 1] += quant_error * 1 / 8;
    }

    // Bottom neighbours
    if(y < y_size - 1){
        pixels[y + 1][x] += quant_error * 1 / 8;
    }
    if(y < y_size - 2){
        pixels[y + 2][x] += quant_error * 1 / 8;
    }

    // Bottom Left neighbours
    if(x >= 1 && y < y_size - 1){
        pixels[y + 1][x - 1] += quant_error * 1 / 8;
    }
}

void dither(uint8_t** pixels, int x_size, int y_size, enum DitherAlgorithm algorithm){
    for (int y = 1; y < y_size; y++) {
        for (int x = 0; x < x_size; x++) {
            uint8_t oldpixel = pixels[y][x];
            uint8_t newpixel = find_closest_palette_color(oldpixel);
            uint8_t quant_error = oldpixel - newpixel;
            pixels[y][x] = newpixel;

            switch(algorithm){
                case FLOYD_STEINBERG:
                    floydSteinbergDither(pixels, y, x, y_size, x_size, quant_error);
                    break;
                case ATKINSON:
                    atkinsonDither(pixels, y, x, y_size, x_size, quant_error);
                    break;
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

int itterativeDither(uint8_t** pixels, char* outputFile, int x_size, int y_size, int passes, enum DitherAlgorithm algorithm){
    //uint8_t** pixels = readImage(inputFile, x_size, y_size);
    for(int p = 0; p < passes; p++){
        dither(pixels, x_size, y_size, algorithm);
        interpolateImage(pixels, x_size, y_size);
    }
    writeImage(pixels, x_size, y_size, outputFile);

    return 0;
}

void freePixels(uint8_t** pixels, int height) {
    for (int i = 0; i < height; i++) {
        free(pixels[i]);
    }
    free(pixels);
}