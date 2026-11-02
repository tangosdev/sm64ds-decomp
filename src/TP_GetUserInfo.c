/* NitroSDK SPI_tp.c TP_GetUserInfo, an older revision of pret's (no clear
 * on failure): the user calibration points in the NVRAM copy at
 * 0x027ffc80 + 0x58; 0 when all four raw coordinates are zero, else
 * whether TP_CalcCalibrateParam accepts them. */
#include "private/nvram_config.h"

extern unsigned int TP_CalcCalibrateParam(int calibrate, int rawX1, int rawY1, int dx1, int dy1,
                                          int rawX2, int rawY2, int dx2, int dy2);
int TP_GetUserInfo(int calibrate) {
    NVRAMConfigData *p = (NVRAMConfigData *)0x027ffc80;
    unsigned short rawX1, rawY1, rawX2, rawY2;
    unsigned char dx1, dy1, dx2, dy2;
    rawX1 = p->tp.raw_x1;
    rawY1 = p->tp.raw_y1;
    dx1 = p->tp.dx1;
    dy1 = p->tp.dy1;
    rawX2 = p->tp.raw_x2;
    rawY2 = p->tp.raw_y2;
    dx2 = p->tp.dx2;
    dy2 = p->tp.dy2;
    if (rawX1 == 0 && rawX2 == 0 && rawY1 == 0 && rawY2 == 0) {
        return 0;
    }
    return TP_CalcCalibrateParam(calibrate, rawX1, rawY1, dx1, dy1, rawX2, rawY2, dx2, dy2) == 0;
}
