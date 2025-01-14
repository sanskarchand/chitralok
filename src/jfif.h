#ifndef JFIF_H
#define JFIF_H

#include <stdio.h>
#include <stdint.h>

//pixel density format
enum PJPdf {
    PJ_PDF_NOUNIT = 0x00,
    PJ_PDF_PPI = 0x01,
    PJ_PDF_PPC = 0x02
};

struct parsed_jfif {
    unsigned char pj_ver[2];    //JFIF ver
    uint8_t pj_pdf;             //pixel density format 
    uint16_t pj_dens[2];

    uint8_t pj_thumb_size[2];   //embedded thumbnail dimensions
    uint8_t *pj_thumb_data;     //NOTE: user responsible for free();

    uint8_t *pj_data;           //actual image data
    uint8_t pj_img_size;
};

int process_jfif(FILE *jpegFile, struct parsed_jfif *parsed,
        void (*fDebug) (uint8_t *, int) );

#endif //JFIF_H
