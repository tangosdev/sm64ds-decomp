// Actor::MakeVanishLuigiWork(CylinderClsn&): clears bit 1 of the cylinder's
// flags (+0x18), then re-sets it if the closest player has +0x6fb set
// (vanish-cap state). Callee: _ZN5Actor13ClosestPlayerEv @ 0x02010ad8.
typedef unsigned int u32;
typedef unsigned char u8;

extern void *_ZN5Actor13ClosestPlayerEv(void *self);

void _ZN5Actor19MakeVanishLuigiWorkER12CylinderClsn(void *self, char *clsn)
{
    void *player;

    *(u32 *)(((long long)(int)(clsn + 0x18)) & 0xFFFFFFFFFFFFFFFFLL) &= ~2;

    player = _ZN5Actor13ClosestPlayerEv(self);
    if (player == 0)
        return;

    if (*((u8 *)player + 0x6fb) != 0) {
        *(u32 *)(((unsigned long long)(u32)(clsn + 0x18)) & 0xFFFFFFFFFFFFFFFFULL) |= 2;
    }
}
