#include <stdio.h>
#include <omp.h>
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

    //skip metadata
    while (fgetc(f) != '\n'); //skip first line P2
    while (fgetc(f) != '\n'); //skip second line 128 128 dimensions
    while (fgetc(f) != '\n'); //skip third line 255 width

    //read in pixels
    for (int y = 0; y < Y_SIZE; y++) {
        for (int x = 0; x < X_SIZE; x++) {
            pixels[x][y] = 0;
            char c = fgetc(f);
            while(c != '\n'){
                //read in number digit by digit
                c = atoi(&c);
                pixels[x][y]*= 10;
                pixels[x][y] += c;
                c = fgetc(f);
            }
        }
    }
    omp_lock_t* locks = malloc(sizeof(omp_lock_t) * X_SIZE * Y_SIZE);
    for (int i = 0; i < X_SIZE * Y_SIZE; i++) {
        omp_init_lock(&locks[i]);
    }
    
    #pragma omp parallel for schedule(static)
    for (int y = 1; y < Y_SIZE; y++) {
        for (int x = 0; x < X_SIZE; x++) {
            uint8_t oldpixel = pixels[x][y];
            uint8_t newpixel = find_closest_palette_color(oldpixel);
            uint8_t quant_error = oldpixel - newpixel;
            //set_lock will suspend the thread until the lock is available
            omp_set_lock(&locks[x * Y_SIZE + y]);
            pixels[x][y] = newpixel;
            omp_unset_lock(&locks[x * Y_SIZE + y]);

            if(x < X_SIZE-1){
                omp_set_lock(&locks[(x+1) * Y_SIZE + y]);
                pixels[x + 1][y] += quant_error * 7 / 16;
                omp_unset_lock(&locks[(x+1) * Y_SIZE + y]);
            }
 
            if(x > 0 && y < Y_SIZE - 1){
                omp_set_lock(&locks[(x-1) * Y_SIZE + y + 1]);
                pixels[x - 1][y + 1] += quant_error * 3 / 16;
                omp_unset_lock(&locks[(x-1) * Y_SIZE + y + 1]);
            }
 
            if(y < Y_SIZE - 1){
                omp_set_lock(&locks[x * Y_SIZE + y + 1]);
                pixels[x][y + 1] += quant_error * 5 / 16;
                omp_unset_lock(&locks[x * Y_SIZE + y + 1]);
            }
   
            if(x < X_SIZE - 1 && y < Y_SIZE - 1){
                omp_set_lock(&locks[(x+1) * Y_SIZE + y + 1]);
                pixels[x + 1][y + 1] += quant_error * 1 / 16;
                omp_unset_lock(&locks[(x+1) * Y_SIZE + y + 1]);
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