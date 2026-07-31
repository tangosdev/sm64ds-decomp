typedef unsigned int u32;

extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern int _ZN6Player7IsInAirEv(void *player);

void func_02037318(char *self)
{
    const int *src;

    *(u32 *)(((int)self + 0x10)) &= ~0x100;
    if (_ZNK12WithMeshClsn10IsOnGroundEv(self) == 0)
        return;
    if (_ZN6Player7IsInAirEv(*(void **)(self + 0x14)))
        return;
    *(u32 *)(((int)(self + 0x10))) |= 0x100;
    src = (const int *)(((int)self + 0x11c));
    *(int *)(self + 0x1ac) = src[0];
    *(int *)(self + 0x1b0) = src[1];
    *(int *)(self + 0x1b4) = src[2];
}
