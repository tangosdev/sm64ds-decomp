/* NitroSDK SPI_tp.c TP_GetCalibratedPoint(disp, raw): copy the sample while
 * uncalibrated, else keep touch/validity and scale x/y by the calibration,
 * clamped to the 256x192 screen. The calibration words used to carry their own
 * symbol row, data_020a80e4, although this function reads the calibrated flag
 * out of the same block one line earlier; see include/nitro/spi/tp.h. */
#include "nitro/spi/tp.h"

void TP_GetCalibratedPoint(TPData *disp, TPData *raw) {
    int *p;
    int t;
    if (data_020a80cc.calibrated == 0) {
        *disp = *raw;
        return;
    }
    p = data_020a80cc.calibration;
    disp->touch = raw->touch;
    disp->validity = raw->validity;
    if (raw->touch == 0) {
        disp->x = 0;
        disp->y = 0;
        return;
    }
    disp->x = (int)(((long long)p[2] * ((long long)(raw->x << 2) - (long long)p[0])) >> 22);
    t = (short)disp->x;
    if (t < 0) {
        disp->x = 0;
    } else if (t > 0xff) {
        disp->x = 0xff;
    }
    disp->y = (int)(((long long)p[5] * ((long long)(raw->y << 2) - (long long)p[3])) >> 22);
    t = (short)disp->y;
    if (t < 0) {
        disp->y = 0;
        return;
    }
    if (t > 0xbf) {
        disp->y = 0xbf;
    }
}
