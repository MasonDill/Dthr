#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

enum DitherAlgo {
    FLOYD_STEINBERG,
    ATKINSON,
    BLACK_WHITE,
    HALF_TONE
};

enum DitherType {
    ERROR_DIFFUSION,
    ORDERED_DITHER
};

struct DitheringAlgorithm {
    enum DitherAlgo algorithm;
    enum DitherType type;
    char* name;
};

struct DitheringAlgorithm ditheringAlgorithms[] = {
    {FLOYD_STEINBERG, ERROR_DIFFUSION, "floyd-steinberg"},
    {ATKINSON, ERROR_DIFFUSION, "atkinson"},
    {BLACK_WHITE, ORDERED_DITHER, "black-white"},
    {HALF_TONE, ORDERED_DITHER, "half-tone"}
};

int find_closest_palette_color(int oldpixel, int threshold) {
    //compare to threshold
    return oldpixel > threshold ? 1 : 0;
}

void floydSteinbergDither(uint8_t** pixels, int y, int x, int y_size, int x_size, uint8_t quant_error){
    // Right neighbours
    if(x < x_size-1){
        pixels[y][x + 1] += quant_error * 7 / 16;
    }

    // Bottom Left neighbour
    if(x > 0 && y < y_size - 1){
        pixels[y + 1][x - 1] += quant_error * 3 / 16;
    }

    // Bottom neighbour
    if(y < y_size - 1){
        pixels[y + 1][x] += quant_error * 5 / 16;
    }

    // Bottom Right neighbour
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

void errorDiffusionDither(uint8_t** pixels, int x_size, int y_size, enum DitherAlgo algorithm){
    void (*ditherDiffusionFunction)(uint8_t**, int, int, int, int, uint8_t);

    switch(algorithm){
        case FLOYD_STEINBERG:
            ditherDiffusionFunction = floydSteinbergDither;
            break;
        case ATKINSON:
            ditherDiffusionFunction = atkinsonDither;
            break;
        default:
            printf("Invalid error diffusion algorithm\n");
            return;
    }

    for (int y = 1; y < y_size; y++) {
        for (int x = 0; x < x_size; x++) {
            uint8_t oldpixel = pixels[y][x];
            uint8_t newpixel = find_closest_palette_color(oldpixel, 255/2);
            uint8_t quant_error = oldpixel - newpixel;
            pixels[y][x] = newpixel;

            ditherDiffusionFunction(pixels, y, x, y_size, x_size, quant_error);
        }
    }
}

const uint8_t MAP_X_SIZE = 4;
const uint8_t MAP_Y_SIZE = 4;

uint8_t** blackWhiteMapTile(){
    uint8_t** map = (uint8_t**) malloc(MAP_Y_SIZE * sizeof(uint8_t*));
    for (int i = 0; i < MAP_X_SIZE; i++) {
        map[i] = (uint8_t*) malloc(MAP_X_SIZE * sizeof(uint8_t)); // allocate

        for (int j = 0; j < MAP_X_SIZE; j++) {
            map[i][j] = 128; // assign
        }
    }

    return map;
}

uint8_t** halfToneMapTile(){
    uint8_t** map = (uint8_t**) malloc(MAP_Y_SIZE * sizeof(uint8_t*));
    for (int i = 0; i < MAP_X_SIZE; i++) {
        map[i] = (uint8_t*) malloc(MAP_X_SIZE * sizeof(uint8_t)); // allocate

        for (int j = 0; j < MAP_X_SIZE; j++) {
            if(i > 1 && i < 3 && j > 1 && j < 3){
                map[i][j] = 255;
            } else {
                map[i][j] = 127;
            }
        }
    }

    return map;
}

uint8_t** createThresholdMapTile(enum DitherAlgo algorithm){
    switch(algorithm){
        case BLACK_WHITE:
            return blackWhiteMapTile();
        case HALF_TONE:
            return halfToneMapTile();
        default:
            printf("Invalid ordered dithering algorithm\n");
            return NULL;
    }
}

uint8_t** createThresholdMap(enum DitherAlgo algorithm, int x_size, int y_size){
    uint8_t** map = (uint8_t**) malloc(y_size * sizeof(uint8_t*));
    uint8_t** tile = createThresholdMapTile(algorithm);

    for (int i = 0; i < y_size; i++) {
        map[i] = (uint8_t*) malloc(x_size * sizeof(uint8_t));

        for (int j = 0; j < x_size; j++) {
            map[i][j] = tile[i % MAP_Y_SIZE][j % MAP_X_SIZE];
        }
    }

    return map;
}

void orderedDither(uint8_t** pixels, int x_size, int y_size, enum DitherAlgo algorithm){
    uint8_t** map = createThresholdMap(algorithm, x_size, y_size);

    for (int y = 0; y < y_size; y++) {
        for (int x = 0; x < x_size; x++) {
            int threshold = map[y % MAP_Y_SIZE][x % MAP_X_SIZE];
            pixels[y][x] = !find_closest_palette_color(pixels[y][x], threshold);
        }
    }
}

void interpolateImage(uint8_t** pixels, int x_size, int y_size, bool negate){
    for (int y = 0; y < y_size; y++) {
        for (int x = 0; x < x_size; x++) {
            pixels[y][x] = negate ? !pixels[y][x] : pixels[y][x];
            pixels[y][x] -= 1; // change codomain from [0, 1] to [255, 0]
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

struct DitheringAlgorithm getDitheringAlgorithm(char* algorithm){
    for(int i = 0; i < sizeof(ditheringAlgorithms)/sizeof(ditheringAlgorithms[0]); i++){
        if(strcmp(ditheringAlgorithms[i].name, algorithm) == 0){
            return ditheringAlgorithms[i];
        }
    }

    // Not found
    struct DitheringAlgorithm error = {-1, -1, "ERROR"};
    return error;
}

int itterativeDither(uint8_t** pixels, char* outputFile, int x_size, int y_size, int passes, char* algorithm, bool negate){
    void (*ditherAlgoFunction)(uint8_t**, int, int, enum DitherAlgo);

    struct DitheringAlgorithm ditheringAlgorithm = getDitheringAlgorithm(algorithm);
    switch(ditheringAlgorithm.type){
        case ERROR_DIFFUSION:
            ditherAlgoFunction = errorDiffusionDither;
            break;
        case ORDERED_DITHER:
            ditherAlgoFunction = orderedDither;
            break;
        default:
            printf("Invalid dithering algorithm\n");
            return -1;
    }

    //uint8_t** pixels = readImage(inputFile, x_size, y_size);
    for(int p = 0; p < passes; p++){
        ditherAlgoFunction(pixels, x_size, y_size, ditheringAlgorithm.algorithm);
        interpolateImage(pixels, x_size, y_size, negate);
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