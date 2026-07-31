typedef unsigned char u8;
typedef unsigned int u32;

extern unsigned char *_ZN5Actor13ClosestPlayerEv(void *self);

void func_0200f760(void *self, char *actor)
{
    unsigned char *p;

    *(u32 *)(((int)actor + 0x18)) &= ~2;
    p = _ZN5Actor13ClosestPlayerEv(self);
    if (p == 0) return;
    if (p[0x6fb] != 0) {
        *(u32 *)(((int)(actor + 0x18))) |= 2;
    }
}
