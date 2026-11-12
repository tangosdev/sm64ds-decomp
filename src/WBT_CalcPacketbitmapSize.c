#include "nitro/wbt.h"

int func_02065bd0(void);

int WBT_CalcPacketbitmapSize(int a) {
    int d = func_02065bd0();
    int n = (a % d != 0 ? 1 : 0) + a / d;
    int r = (n & 0x1f ? 1 : 0) + (n >> 5);
    return r << 2;
}
