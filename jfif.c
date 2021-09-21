#include <stdlib.h>
#include <stdint.h>
#include "jfif.h"
#include "logging.h"

int process_jfif(FILE *jpegFile)
{
    fseek(jpegFile, 0L, SEEK_END);
    long size = ftell(jpegFile);
    LOG_INFO("File size is %lu bytes", size);
    fseek(jpegFile, 0L, SEEK_SET);
    
    uint8_t *bytes = (uint8_t *) malloc(sizeof(uint8_t) * size);
    if (bytes == NULL) {
        LOG_ERROR("Could not allocate sufficient memory"); 
        return -1; 
    }
    
    fread(bytes, size, sizeof(uint8_t), jpegFile);


    if (bytes[0] == 0xFF && bytes[1] == 0xD8) {
        printf("JPEG!\n");
    }  else {
        LOG_INFO("Error:%s(%s, %d): not a JPEG\n");
        free(bytes);
        return -1;
    }

    //printf("Val %x %x\n", (unsigned char) try[0], (unsigned char) try[1]);
    

    free(bytes);

}
