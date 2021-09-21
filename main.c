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

FILE *_log_stream;

void exit_cleanup() {
    LOG_CLOSE()
}

void process_jpeg(FILE *jpegFile)
{
    if(process_jfif(jpegFile) == -1) {
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
