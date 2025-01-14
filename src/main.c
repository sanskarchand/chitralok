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
#include "logging.h"
#include "jfif.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL_Window *gWindow = NULL;
SDL_Surface *gSurf = NULL;
SDL_Renderer *gRenderer = NULL;

uint32_t *debugPixels;

FILE *_log_stream;

void exit_cleanup() {
    LOG_CLOSE()
}

//RGB format
void set_pixel_data( uint8_t *data, int num_pixels)
{
    LOG_INFO("set_pixel_data: num_pixels = %d\n", num_pixels)

    for (int i = 0; i < num_pixels; i++)  {
        *debugPixels++ = *(data+i) << 2 | *(data+i+2) << 1 | *(data+i+3);
    }
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
    SDL_Event event;
    struct parsed_jfif parsed;
    
    //init SDL
    if (init() != 0) {
        return -1;
    }
    // temp img dims
    int im_w = 640;
    int im_h = 480;

    SDL_Texture* framebuffer = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, im_w, im_h);

    //using 32-bits for alignment, even when only 24 are necessary
    debugPixels = (uint32_t*) malloc(im_w * im_h * sizeof(uint32_t));



    char *file = NULL;
    FILE *jpegFile; 
    char *path = NULL;

    LOG_INIT_STDOUT()


    if (argc < 2) {
        LOG_INFO("No path provided; Using lenna")
        //Try to use lenna
#if defined(__unix__)
        char *home = getenv("HOME");
        path = (char *) malloc(strlen(home) + sizeof("/Pictures/lenna.jpg") + 1);
        if (path) {
            sprintf(path, "%s%s", home, "/Pictures/lenna.jpg");
        } else {
        }
#elif defined(__WIN32__)
        char *home = getenv("USERPROFILE");
        path = (char *) malloc(strlen(home) + sizeof("\\Pictures\\lenna.jpg") + 1);
        if (path) {
            sprintf(path, "%s%s", home, "\\Pictures\\lenna.jpg");
        }

        jpegFile = fopen(path, "rb");
        free(path);
#endif

    } else {
        LOG_INFO("argv[1] used for path")
        jpegFile = fopen(argv[1], "rb");
    }
    


    if (!jpegFile) {
        fprintf(stderr, "Error: Could not open file(%s)\n", strerror(errno));
        exit_cleanup();
        return -1;
    }
    
    LOG_INFO("About to process the jpeg")

    // Step 1: Parse JFIF and store data
    if(process_jfif(jpegFile, &parsed, set_pixel_data) == -1) {
        return 1;
    }

    // Step 2: Huffman decoding


    fclose(jpegFile);
    
    /*
    while (1) {
        SDL_PollEvent(&event);                  //TODO: remove this, just use SDL_PumpEvents() (below)
        if (event.type == SDL_QUIT) {
            break;
        }

        //update texture
        SDL_UpdateTexture(framebuffer , NULL, debugPixels, im_w * sizeof (uint32_t));


        SDL_RenderClear(gRenderer);
        SDL_RenderCopy(gRenderer, framebuffer , NULL, NULL);
        SDL_RenderPresent(gRenderer);
    }

    SDL_DestroyWindow(gWindow);
    SDL_Quit();
    */

    return 0;

}
