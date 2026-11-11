/* NitroSDK SPI_tp.c TP_GetUserInfo, an older revision of pret's (no clear
 * on failure): the user calibration points in the firmware settings the
 * boot code copied to the system work area; 0 when all four raw
 * coordinates are zero, else whether TP_CalcCalibrateParam accepts them. */
#include "nitro/os/systemWork.h"
#include "nitro/spi/userInfo.h"
#include "nitro/spi/tp.h"

int TP_GetUserInfo(TPCalibrateParam *calibrate) {
    NVRAMConfig *p = (NVRAMConfig *)(OS_GetSystemWork()->nvramUserInfo);
    unsigned short rawX1, rawY1, rawX2, rawY2;
    unsigned char dx1, dy1, dx2, dy2;
    rawX1 = p->ncd.tp.raw_x1;
    rawY1 = p->ncd.tp.raw_y1;
    dx1 = p->ncd.tp.dx1;
    dy1 = p->ncd.tp.dy1;
    rawX2 = p->ncd.tp.raw_x2;
    rawY2 = p->ncd.tp.raw_y2;
    dx2 = p->ncd.tp.dx2;
    dy2 = p->ncd.tp.dy2;
    if (rawX1 == 0 && rawX2 == 0 && rawY1 == 0 && rawY2 == 0) {
        return 0;
    }
    return TP_CalcCalibrateParam(calibrate, rawX1, rawY1, dx1, dy1, rawX2, rawY2, dx2, dy2) == 0;
}
