#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

//change for different image sizes
#define INPUT_FILE_IDX 1
#define X_SIZE_IDX 2
#define Y_SIZE_IDX 3
#define OUTPUT_FILE_IDX 4
#define PASSES_IDX 5

int find_closest_palette_color(int oldpixel){
    //converts from 0-255 grayscale to 0-1, black-white
    return round(((double) oldpixel) / 255);
}

int* sanityCheckArgs(int argc, char *argv[]){
    char input[] = "<input.data>";
    char output[] = "<output.pgm>";
    char x_size[] = "<x_size>";
    char y_size[] = "<y_size>";
    char passes[] = "[<passes>]";

    char** args = (char**)malloc(5 * sizeof(char*));
    args[INPUT_FILE_IDX] = input;
    args[X_SIZE_IDX] = x_size;
    args[Y_SIZE_IDX] = y_size;
    args[OUTPUT_FILE_IDX] = output;
    args[PASSES_IDX] = passes;

    if(argc < 5 || argc > 6){
        printf("Usage: %s %s %s %s %s %s\n", argv[0], input, x_size, y_size, output, passes);
        exit(1);
    }

    FILE *f = fopen(argv[INPUT_FILE_IDX], "r");
    if(f == NULL){
        printf("File %s does not exist\n", argv[1]);
        exit(1);
    }
    fclose(f);

    return 0;
}

int main(int argc, char *argv[]) {
    sanityCheckArgs(argc, argv);

    char* inputFile = argv[INPUT_FILE_IDX];
    int x_size = atoi(argv[X_SIZE_IDX]);
    char* outputFile = argv[OUTPUT_FILE_IDX];
    int y_size = atoi(argv[Y_SIZE_IDX]);
    int passes = 1; // default to 1 pass
    
    if(argc == 6){
        passes = atoi(argv[PASSES_IDX]);
    }

    uint8_t pixels[x_size][y_size];

    //read pgm file
    FILE *f = fopen(argv[1], "r");

    //read in pixels
    for (int y = 0; y < y_size; y++) {
        for (int x = 0; x < x_size; x++) {
            pixels[x][y] = 0;
            char c = fgetc(f);

            int pixelCounter = 0;
            while(c != ' ' && c != '\n'){
                //read in number digit by digit
                c = atoi(&c);
                pixels[x][y]*= 10;
                pixels[x][y] += c;
                c = fgetc(f);

                if(pixelCounter > 3 || pixels[x][y] > 255){
                    printf("Invalid pixel value. Please make sure the provided file was given in the correct format.\n");
                    exit(1);
                }
            }
        }
    }
    fclose(f);

    for(int p = 0; p < passes; p++){
        for (int y = 1; y < y_size; y++) {
            for (int x = 0; x < x_size; x++) {
                uint8_t oldpixel = pixels[x][y];
                uint8_t newpixel = find_closest_palette_color(oldpixel);
                uint8_t quant_error = oldpixel - newpixel;
                pixels[x][y] = newpixel;

                if(x < x_size-1){
                    pixels[x + 1][y] += quant_error * 7 / 16;
                }
    
                if(x > 0 && y < y_size - 1){
                    pixels[x - 1][y + 1] += quant_error * 3 / 16;
                }
    
                if(y < y_size - 1){
                    pixels[x][y + 1] += quant_error * 5 / 16;
                }
    
                if(x < x_size - 1 && y < y_size - 1){
                    pixels[x + 1][y + 1] += quant_error * 1 / 16;
                }
            }
        }

        //repolate image
        for (int y = 0; y < y_size; y++) {
            for (int x = 0; x < x_size; x++) {
                pixels[x][y] += 255;
            }
        }
    }
    

    //write dithered image to file
    FILE *f5 = fopen(argv[OUTPUT_FILE_IDX], "w");
    fprintf(f5, "P2\n%d %d\n", x_size, y_size); //P2 format, dimensions
    fprintf(f5, "255\n"); //255 bit depth
    for (int y = 0; y < y_size; y++) {
        for (int x = 0; x < x_size; x++) {
            fprintf(f5, "%d\n", pixels[x][y]);
        }
    }
    fclose(f5);

  return 0;
}