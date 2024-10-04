#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

//change for different image sizes
#define X_SIZE 128
#define Y_SIZE 128

uint8_t pixels[X_SIZE][Y_SIZE];

int find_closest_palette_color(int oldpixel){
    //converts from 0-255 grayscale to 0-1, black-white
    return round(((double) oldpixel) / 255);
}

int main(int argc, char *argv[]) {
    
    //read pgm file
    FILE *f = fopen(argv[1], "r");

    //read in pixels
    for (int y = 0; y < Y_SIZE; y++) {
        for (int x = 0; x < X_SIZE; x++) {
            pixels[x][y] = 0;
            char c = fgetc(f);
            while(c != ' ' && c != '\n'){
                //read in number digit by digit
                c = atoi(&c);
                pixels[x][y]*= 10;
                pixels[x][y] += c;
                c = fgetc(f);
            }
        }
    }

    for (int y = 1; y < Y_SIZE; y++) {
        for (int x = 0; x < X_SIZE; x++) {
            uint8_t oldpixel = pixels[x][y];
            uint8_t newpixel = find_closest_palette_color(oldpixel);
            uint8_t quant_error = oldpixel - newpixel;
            pixels[x][y] = newpixel;

            if(x < X_SIZE-1){
                pixels[x + 1][y] += quant_error * 7 / 16;
            }
 
            if(x > 0 && y < Y_SIZE - 1){
                pixels[x - 1][y + 1] += quant_error * 3 / 16;
            }
 
            if(y < Y_SIZE - 1){
                pixels[x][y + 1] += quant_error * 5 / 16;
            }
   
            if(x < X_SIZE - 1 && y < Y_SIZE - 1){
                pixels[x + 1][y + 1] += quant_error * 1 / 16;
            }
        }
    }

    //write dithered image to file
    FILE *f5 = fopen(argv[2], "w");
    fprintf(f5, "P2\n%d %d\n", X_SIZE, Y_SIZE); //P2 format, dimensions
    fprintf(f5, "255\n"); //255 bit depth
    for (int y = 0; y < Y_SIZE; y++) {
        for (int x = 0; x < X_SIZE; x++) {
            //repolate
            pixels[x][y] += 255;

            fprintf(f5, "%d\n", pixels[x][y]);
        }
    }
    fclose(f5);

  return 0;
}