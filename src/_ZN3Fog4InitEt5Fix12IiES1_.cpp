//cpp
// @symbol _ZN3Fog4InitEt5Fix12IiES1_
/* recovered: named members + shared header */
#include "Fog.h"
extern "C" void _ZN3Fog4InitEt5Fix12IiES1_(struct Fog *self, unsigned short color, int nearv, int farv) {
    int dist;
    int step;
    int i;
    char* p;

    self->unk_020 = 1;
    dist = 0x28000 - (nearv << 4);
    self->unk_024 = color;
    self->unk_021 = 6;
    self->unk_022 = 0;
    if (dist >= 0x80000)
        dist = 0x7ffff;
    step = (0x100000 - (farv << 4) - dist) >> 5;
    i = 0;
    p = ((char*)self);
    while (i < 0x20) {
        if (dist <= 0) {
            *p = 0;
        } else if (dist > 0x80000) {
            *p = 0x7f;
        } else {
            *p = (unsigned char)(dist >> 12);
        }
        i++;
        p++;
        dist += step;
    }
}