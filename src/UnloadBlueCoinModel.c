#include "types.h"
struct SharedFilePtr { u32 data[4]; };

extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);

extern struct SharedFilePtr BlueCoinModelFile1;
extern struct SharedFilePtr BlueCoinModelFile2;

void UnloadBlueCoinModel(void)
{
    _ZN13SharedFilePtr7ReleaseEv(&BlueCoinModelFile1);
    _ZN13SharedFilePtr7ReleaseEv(&BlueCoinModelFile2);
}
