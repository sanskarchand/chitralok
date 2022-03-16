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

void process_jpeg(FILE *jpegFile)
{
    if(process_jfif(jpegFile, set_pixel_data) == -1) {
        return;
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
    char *path = NULL;

    LOG_INIT_FILE()


    if (argc < 2) {
        LOG_INFO("No path provided; Using lenna")
        //Try to use lenna
#if defined(__unix__)
        char *home = getenv("HOME");
        path = (char *) malloc(strlen(home) + sizeof("/Pictures/lenna.jpg") + 1);
        if (path) {
            sprintf(path, "%s%s", home, "/Pictures/lenna.jpg");
        }
#elif defined(__WIN32__)
        char *home = getenv("USERPROFILE");
        path = (char *) malloc(strlen(home) + sizeof("\\Pictures\\lenna.jpg") + 1);
        if (path) {
            sprintf(path, "%s%s", home, "\\Pictures\\lenna.jpg");
        }
#endif

    } else {
        LOG_INFO("argv[1] used for path")
        path = argv[1];
    }
    
    FILE *jpegFile = fopen(path, "rb");


    // don't forget to free path
    if (path) { 
        free(path);
    }
    if (!jpegFile) {
        fprintf(stderr, "Error: Could not open file(%s)\n", strerror(errno));
        exit_cleanup();
        return -1;
    }
    
    LOG_INFO("About to process the jpeg")
    process_jpeg(jpegFile);

    fclose(jpegFile);
        
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

    return 0;

}
