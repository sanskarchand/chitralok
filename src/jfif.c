#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "jfif.h"
#include "logging.h"

uint8_t *bytes;


// quantization table copied from Tanenbaun
//  TODO: move elsewhere and tweak table
const uint8_t QUANT_TABLE[8][8] = {
    {150, 80, 20, 4, 1, 0, 0},
    {92, 75, 18, 3, 1, 0, 0},
    {26, 19, 13, 2, 1, 0, 0},
    {3, 2, 2, 1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    
};

int die()
{
    free(bytes);
    return -1;
}

// visualization aid
static inline void peek_hex(uint8_t *ptr)
{

    int i = 0;
    while (i < 20) {
        printf("%02X ", *ptr++);
        i++;
    }
    printf("\n");

}

static inline void parse_thumbnail(
    struct parsed_jfif *parsed,
    uint8_t *cur
){
    memcpy(parsed->pj_thumb_size, cur, 2);
    int n = parsed->pj_thumb_size[0] * parsed->pj_thumb_size[1];
    cur += 2;

    LOG_INFO("Thumb dims are %dx%d", parsed->pj_thumb_size[0], parsed->pj_thumb_size[1])
    LOG_INFO("Allocating %d bytes for thumb data", 3 * n)
    parsed->pj_thumb_data = (uint8_t *) malloc(3 * n);  //NOTE: check for err.
    memcpy(parsed->pj_thumb_data, cur, 3 * n);
    cur += 3 * n;
}

int process_jfif(
    FILE *jpegFile, 
    struct parsed_jfif* parsed, 
    void (*fDebug) (uint8_t *, int) 
){


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
    
    LOG_INFO("Starting JFIF-APP0 segment");

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
    memcpy(parsed->pj_ver, cur, 2);
    cur += 2;

    
    // pixel density format; forgoing error checking for now
    parsed->pj_pdf = *cur;
    cur += 1;
    
    //x and y density
    memcpy(parsed->pj_dens, cur, 4);
    assert(parsed->pj_dens[0] != 0 && parsed->pj_dens[1] != 0);
    cur += 4;

    //embedded thumbnail stuff
    parsed->pj_thumb_data = NULL;
    // check if thumbnail exists
    if (*cur == 0){
        LOG_INFO("No thumbnail in APP0 marker segment.")
        cur += 2; // skip to APP0 extension
    } else {
       
        parse_thumbnail(parsed, cur);
        LOG_INFO("Debug-drawing thumbnail")
        int n = parsed->pj_thumb_size[0] * parsed->pj_thumb_size[1];
        fDebug(parsed->pj_thumb_data, n);
    }

    // TODO: find FFC4, the Huffman table



    // ignore other segments e.g. exif APP1 or photoshop's APP13
    // go straight to SOS
    size_t offset = cur - bytes;
    LOG_INFO("Skipping to SOS segment")

    while (offset < size) {
        if (*cur == 0xFF) {
            if (*(cur+1) == 0xDA) {
                LOG_INFO("SOS located at offset %zu", offset)
                break; 
            } else if (*(cur+1) != 0x00 && *(cur+1) != 0xFF) {
                LOG_INFO("Skipping segment with header %02X%02X at offset %zu", *cur, *(cur+1), offset);
            }
        }

        cur++;
        offset++;
    }

    //get offset of the last EOI marker (FF D9)

    //NOTE: for some reason, even though my test image has no thumbnail,
    //I get two occurences of FF D9.
    //There is the option of directly reading to EOF, but some jpeg files
    //may have a tailer
    size_t offset_end;
    size_t cnt = offset;
    while (cnt <  size) {
        if (*cur == 0XFF && *(cur+1) == 0xD9) {
            offset_end = cur - bytes;
        }
        cur++; 
        cnt++;
    }


    LOG_INFO("Final EOI offset is %zu", offset_end)
    
    //copy img data
    size_t total_size = offset_end - offset + 1 - 4; //+1 for count, -4 to exclude markers
    parsed->pj_data = (uint8_t*) malloc(total_size);

    memcpy(parsed->pj_data, bytes + offset, total_size);
    parsed->pj_img_size = total_size;
    
    LOG_INFO("Copied total %zu bytes of imgdata", total_size)

    peek_hex(bytes + offset + total_size - 20);

    printf("%02x %02x %02x %02x\n", parsed->pj_data[0], parsed->pj_data[1], parsed->pj_data[total_size-2],
            parsed->pj_data[total_size-1]);

    free(bytes);


    return 0;

}
