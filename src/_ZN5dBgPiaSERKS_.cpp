//cpp
// @symbol _ZN10ClsnResultaSERKS_
/* recovered: named members + shared header */
#include "ClsnResult.h"
extern "C" char *_ZN10ClsnResultaSERKS_(struct ClsnResult *self, const char *o) {
    self->unk_004 = *(const long long *)(o + 4);
    self->unk_00c = *(const int *)(o + 0xc);
    self->unk_010 = *(const int *)(o + 0x10);
    self->unk_014 = *(const int *)(o + 0x14);
    self->unk_018 = *(const unsigned short *)(o + 0x18);
    self->unk_01a = *(const unsigned short *)(o + 0x1a);
    self->unk_01c = *(const int *)(o + 0x1c);
    self->unk_020 = *(const int *)(o + 0x20);
    self->unk_024 = *(const int *)(o + 0x24);
    return ((char *)self);
}
