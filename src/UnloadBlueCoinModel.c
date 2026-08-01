#include "types.h"
struct SharedFilePtr { u32 data[4]; };

extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);

extern struct SharedFilePtr data_ov002_0210da00;
extern struct SharedFilePtr data_ov002_0210d9c8;

void UnloadBlueCoinModel(void)
{
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210da00);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9c8);
}
