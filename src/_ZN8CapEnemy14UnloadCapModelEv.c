#include "types.h"
struct CapEnemy {
    u8 pad[0x113];
    u8 capID;     // 0x113
};

struct SharedFilePtr { u16 fileID; u8 numRefs; char* filePtr; };

extern void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);

extern struct SharedFilePtr *CapModelFiles[6];

void _ZN8CapEnemy14UnloadCapModelEv(struct CapEnemy *this)
{
    s32 idx = this->capID & 7;
    if (idx >= 6) return;
    _ZN13SharedFilePtr7ReleaseEv(CapModelFiles[idx]);
}
