/* Clear the held and edge bytes of the touch record (gTouchInfo,
   PlayerInput.h) of each player present (func_0203daac). The
   soft reset (func_02023498 in func_0202345c.cpp) runs it every frame
   while a reset is pending. */
#include "PlayerInput.h"
extern int func_0203daac(void);
void func_0203bb14(void) {
    TouchInfo *p = gTouchInfo;
    int i = 0;
    if (func_0203daac() <= 0)
        return;
    do {
        p->held = 0;
        p->edge = 0;
        i++;
        p++;
    } while (i < func_0203daac());
}

/* The calibrated touch point and the user calibration record the TP setup
   writes. One object; data_020a0de0 is an interior label. */
u8 data_020a0dd8[0x10];
