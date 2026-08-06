#include "types.h"
extern void func_ov060_02116518(char *self, unsigned int kind, int a2, int a3);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern int RandomIntInternal(int *seed);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int kind, unsigned int b, Vector3 *pos, void *vtab, int e, int f);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);
extern short data_02082214[];
extern int data_0209e650;
#define ML(p) ((int*)(int)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))
#define MLS(p) ((short*)(int)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))
void func_ov060_02116d78(char *c)
{

    if (*(int*)(c+0x360) < 0x5000) *ML(c+0x360) += 0x255;
    if (*(short*)(c+0x92) > 0x800) *MLS(c+0x92) -= 0x200;

    {
        int i0 = (*(unsigned short*)(c+0x92)) >> 4;
        int i1 = (*(unsigned short*)(c+0x94)) >> 4;
        i0 = i0 * 2;
        i1 = i1 * 2 + 1;
        int s0 = data_02082214[i0];
        int speed = *(int*)(c+0x98);
        int s1 = data_02082214[i1];
        *(int*)(c+0xa4) = (s1 * (speed * s0)) / 896;
    }
    {
        int speed = *(int*)(c+0x98);
        int s0 = data_02082214[((*(unsigned short*)(c+0x92))>>4)*2];
        *(int*)(c+0xa8) = (-speed) * s0;
    }
    {
        int s0 = data_02082214[((*(unsigned short*)(c+0x92))>>4)*2];
        int s1 = data_02082214[((*(unsigned short*)(c+0x94))>>4)*2];
        int speed = *(int*)(c+0x98);
        *(int*)(c+0xac) = (s1 * ((-speed) * s0)) / 896;
    }
    *(int*)(c+0x5c) += *(int*)(c+0xa4);
    *(int*)(c+0x60) += *(int*)(c+0xa8);
    *(int*)(c+0x64) += *(int*)(c+0xac);

    func_ov060_02116518(c, 0x9a, 1, *(int*)(c+0x360) * 0xc);
    if (_ZNK12WithMeshClsn10IsOnGroundEv(c+0x110) != 0) {
        int r = RandomIntInternal(&data_0209e650);
        unsigned int r3 = (unsigned int)r >> 16;
        if (r3 % 17 != 0)
            _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x118, 6, (Vector3*)(c+0x5c), 0, *(signed char*)(c+0xcc), -1);
        _ZN9ActorBase18MarkForDestructionEv(c);
    }
    if (*(unsigned short*)(c+0x374) < 0x3c) return;
    _ZN9ActorBase18MarkForDestructionEv(c);
}
