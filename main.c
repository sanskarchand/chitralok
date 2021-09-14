/* 
 * author: Sanskar Chand
 * date: 2021-06-30
 * references:  1. 'Modern Operating Systems', 3 ed (Tanenbaum, A. S.), Sec 7.3.1
 *              2. https://en.wikipedia.org/wiki/JPEG
 */
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>

#include "utils.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL_Window *gWindow = NULL;
SDL_Surface *gSurf = NULL;
SDL_Renderer *gRenderer = NULL;


void process_jpeg(FILE *jpegFile)
{
    fseek(jpegFile, 0L, SEEK_END);
    long size = ftell(jpegFile);
    fseek(jpegFile, 0L, SEEK_SET);
    
    uint8_t *bytes = (uint8_t *) malloc(sizeof(uint8_t) * size);
    if (bytes == NULL) {
        
    }
    
    fread(bytes, size, sizeof(uint8_t), jpegFile);


    if (bytes[0] == 0xFF && bytes[1] == 0xD8) {
        printf("JPEG!\n");
    }  else {
        THROW_ERR("Error:%s(%s, %d): not a JPEG\n");
        return;
    }


    //printf("Val %x %x\n", (unsigned char) try[0], (unsigned char) try[1]);
    

    free(bytes);

}

int init()
{
    gWindow = SDL_CreateWindow("Chitralok",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            0);

    if (gWindow == NULL) {
        fprintf(stderr, "%%s (%d) : Window creation failed\n", __FILE__, __LINE__);
        return -1;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);


    return 0;
}


int main(int argc, char *argv[])
{

    if (argc < 2) {
        fprintf(stderr, "Error: path to jpeg not provided\n");
        return -1;
    }

    FILE *jpegFile= fopen(argv[1], "rb");
    if (!jpegFile) {
        fprintf(stderr, "Error: Could not open file(%s)\n", strerror(errno));
    }

    process_jpeg(jpegFile);

    fclose(jpegFile);



    SDL_Event event;

    if (init() != 0) {
        return -1;
    }
    
    /*TODO: eventually replace the below with the actual contents of the decoded image.
     * 'Tis but a test as of yet.
     */

    int im_w = 640;
    int im_h = 480;

    SDL_Texture* framebuffer = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, im_w, im_h);
    uint32_t* pixels = (uint32_t*) malloc(im_w * im_h * sizeof(uint32_t));

    for (int i = 0; i < im_w * im_h/2; i++) {
        *(pixels + i) = (uint32_t)(255 << 4 | 255 << 3);
    }
    

    //update texture
    SDL_UpdateTexture(framebuffer , NULL, pixels, im_w * sizeof (uint32_t));
    
    while (1) {
        SDL_PollEvent(&event);                  //TODO: remove this, just use SDL_PumpEvents() (below)
        if (event.type == SDL_QUIT) {
            break;
        }

        SDL_RenderClear(gRenderer);
        SDL_RenderCopy(gRenderer, framebuffer , NULL, NULL);
        SDL_RenderPresent(gRenderer);
    }

    SDL_DestroyWindow(gWindow);
    SDL_Quit();

    return 0;

}
