#include "WmWork.h"

/* NitroSDK WmCheckParentParameter, which WM_SetParentParameter (func_020620b0)
 * calls on its WMParentParam and then ignores: userGameInfoLength at most 0x70
 * (WM_SIZE_USER_GAMEINFO), beaconPeriod 10..1000, channel 1..14, and an MP
 * exchange (the parent's parentMaxSize send plus maxEntry children's
 * childMaxSize replies, with their per-packet overheads) that fits 0x15e0
 * (5600) microseconds. The parameter block is include/WmWork.h's
 * WmParentParam. */
int func_02062024(unsigned char *o)
{
    WmParentParam *pparam = (WmParentParam *)o;
    unsigned short a = pparam->userGameInfoLength;
    if (a > 0x70) return 0;
    a = pparam->beaconPeriod;
    if (a >= 0xa && a <= 0x3e8) {
    } else {
        return 0;
    }
    a = pparam->channel;
    if (a >= 1 && a <= 0xe) {
    } else {
        return 0;
    }
    {
        unsigned short c = pparam->childMaxSize;
        unsigned short b = pparam->parentMaxSize;
        unsigned short d = pparam->maxEntry;
        int idx = ((c + 0x20) << 2) + 0x70;
        int base = 0x14a + ((b + 0x26) << 2);
        int v = d * idx + base;
        return v <= 0x15e0;
    }
}
