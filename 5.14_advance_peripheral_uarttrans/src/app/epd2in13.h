
#ifndef EPD_2IN13_H
#define EPD_2IN13_H

#include <stdint.h>

// Display resolution
#ifdef SIZE_4IN2
#define EPD_2IN13_WIDTH       400
#define EPD_2IN13_HEIGHT      300
#else
#define EPD_2IN13_WIDTH       104
#define EPD_2IN13_HEIGHT      212
#endif

#define EPD_2IN13_BUFFER_WIDTH      ((EPD_2IN13_WIDTH % 8 == 0)? (EPD_2IN13_WIDTH / 8 ): (EPD_2IN13_WIDTH / 8 + 1))
#define EPD_2IN13_BUFFER_HEIGHT     EPD_2IN13_HEIGHT
#define EPD_2IN13_BUFFER_SIZE     (EPD_2IN13_BUFFER_HEIGHT * EPD_2IN13_BUFFER_WIDTH)

static const uint16_t EPD_Buffer_Width = EPD_2IN13_BUFFER_WIDTH;
static const uint16_t EPD_Buffer_Height = EPD_2IN13_BUFFER_HEIGHT; 
static const uint16_t EPD_Buffer_Size = EPD_2IN13_BUFFER_SIZE; 

enum {
    EPD_MODE_BEGIN = 0x0,

    EPD_MODE_BW = 0x1,
    EPD_MODE_BWR = 0x2,
    EPD_MODE_GRAY = 0x3,

    EPD_MODE_END = 0x4,
};
// #define EPD_2IN13_FULL			0
// #define EPD_2IN13_PART			1

void epd_hw_init();

void EPD_2IN13_Init();
void EPD_2IN13_Init_With_LUT(const unsigned char* lut);
void EPD_2IN13_Clear(void);
void EPD_2IN13_Display(const uint8_t *Image);

void EPD_2IN13_PrepareBlkRAM(void);
void EPD_2IN13_PrepareRedRAM(void);
void EPD_2IN13_WriteRAM(const uint8_t *buf, const int len);

void EPD_2IN13_UpdateDisplay(void);

// void EPD_2IN13_DisplayPart(UBYTE *Image);
// void EPD_2IN13_DisplayPartBaseImage(UBYTE *Image);

void EPD_2IN13_Sleep(void);

// funcs
void EPD_Clear(uint8_t tofill);
void EPD_Display();
void EPD_Init_With_Mode(uint8_t mode);

#endif
