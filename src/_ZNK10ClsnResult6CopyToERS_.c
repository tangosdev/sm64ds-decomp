// @symbol _ZNK10ClsnResult6CopyToERS_
/* recovered: named members + shared header */
#include "ClsnResult.h"
void _ZNK10ClsnResult6CopyToERS_(struct ClsnResult *self, char *dst) {
    *(long long *)(dst + 4) = *(const long long *)((const char *)&self->unk_004);
    *(int *)(dst + 0xc) = *(const int *)((const char *)&self->unk_00c);
    *(int *)(dst + 0x10) = *(const int *)((const char *)&self->unk_010);
    *(int *)(dst + 0x14) = *(const int *)((const char *)&self->unk_014);
    *(unsigned short *)(dst + 0x18) = *(const unsigned short *)((const char *)&self->unk_018);
    *(unsigned short *)(dst + 0x1a) = *(const unsigned short *)((const char *)&self->unk_01a);
    *(int *)(dst + 0x1c) = *(const int *)((const char *)&self->unk_01c);
    *(int *)(dst + 0x20) = *(const int *)((const char *)&self->unk_020);
    *(int *)(dst + 0x24) = *(const int *)((const char *)&self->unk_024);
}
