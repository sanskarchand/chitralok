#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "jfif.h"
#include "logging.h"

uint8_t *bytes;

int die()
{
    free(bytes);
    return -1;
}

// visualization aid
void peek_hex(uint8_t *ptr)
{

    int i = 0;
    while (i < 20) {
        printf("%02X ", *ptr++);
        i++;
    }
    printf("\n");

}

int process_jfif(FILE *jpegFile, void (*fDebug) (uint8_t *, int) )
{

    struct parsed_jfif parsed;

    fseek(jpegFile, 0L, SEEK_END);
    long size = ftell(jpegFile);
    LOG_INFO("File size is %lu bytes", size);
    fseek(jpegFile, 0L, SEEK_SET);
    
    bytes = (uint8_t *) malloc(sizeof(uint8_t) * size);
    if (bytes == NULL) {
        LOG_ERROR("Could not allocate sufficient memory"); 
        return -1; 
    }
    
    fread(bytes, size, sizeof(uint8_t), jpegFile);

    //check for SOI segment
    if (bytes[0] == 0xFF && bytes[1] == 0xD8) {
        printf("JPEG!\n");
    }  else {
        //LOG_INFO("Error:%s(%s, %d): not a JPEG\n");
        LOG_INFO("Error: not a JPEG\n");
        die();
    }

    
    uint8_t *cur = bytes + 2;

    //process JFIF-APP0 segment
    if (!(*cur == 0xFF && *(cur+1) == 0xE0)) {
        LOG_INFO("Error: APP0 segment expected");
        die();
    }

    cur += 4;

    char id[5] = { 0 }; 
    memcpy(id, cur, 5);
    assert(strcmp("JFIF", id) == 0 && "JFIF identifier not found in APP0 segment");
    cur += 5;       //chomp identfier and nullptr
    

    //copy the version
    memcpy(parsed.pj_ver, cur, 2);
    cur += 2;

    
    // pixel density format; forgoing error checking for now
    parsed.pj_pdf = *cur;
    cur += 1;
    
    //x and y density
    memcpy(parsed.pj_dens, cur, 4);
    //memcpy(parsed.pj_dens, cur, 2);
    //memcpy(parsed.pj_dens + 1, cur+2, 2);

    assert(parsed.pj_dens[0] != 0 && parsed.pj_dens[1] != 0);
    cur += 4;
    
    //embedded thumbnail stuff
    memcpy(parsed.pj_thumb_size, cur, 2);
    int n = parsed.pj_thumb_size[0] * parsed.pj_thumb_size[1];
    cur += 2;

    parsed.pj_thumb_data = NULL;
    if (n != 0) {
        LOG_INFO("Thumb dims are %dx%d", parsed.pj_thumb_size[0], parsed.pj_thumb_size[1]);
        LOG_INFO("Allocating %d bytes for thumb data", 3 * n);
        parsed.pj_thumb_data = (uint8_t *) malloc(3 * n);  //NOTE: check for err.
        memcpy(parsed.pj_thumb_data, cur, 3 * n);
        cur += 3 * n;
        
        LOG_INFO("Debug-drawing thumbnail");
        fDebug(parsed.pj_thumb_data, n);
      
    }

    // ignore other segments e.g. exif APP1 or photoshop's APP13
    // go straight to SOS
    LOG_INFO("Skipping to SOS segment");
    size_t offset = cur - bytes;

    while (offset < size) {
        if (*cur == 0xFF) {
            if (*(cur+1) == 0xDA) {
                LOG_INFO("SOS located at offset %zu", offset);
                break; 
            } else if (*(cur+1) != 0x00 && *(cur+1) != 0xFF) {
                LOG_INFO("Skipping segment with header %02X%02X at offset %zu", *cur,
                            *(cur+1), offset);
            }
        }

        cur++;
        offset++;
    }

    peek_hex(cur);

    free(bytes);

}
