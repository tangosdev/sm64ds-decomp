typedef unsigned char u8;
struct Vector3 { int x, y, z; };

extern u8 DecIfAbove0_Byte(u8 *p);
extern void _ZN12CylinderClsn5ClearEv(char *c);
extern void _ZN5Actor19DisappearPoofDustAtERK7Vector3(void *self, const struct Vector3 *vec);
extern unsigned int func_020228dc(int x, int y, int z);
extern unsigned int func_0202293c(int x, int y, int z);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN6Player4BurnEv(void *p);
extern void _ZN12CylinderClsn6UpdateEv(char *c);

int func_ov002_020b57a8(char *self)
{
    struct Vector3 pos;
    struct Vector3 v2;
    struct Vector3 dust;

    pos.x = *(int *)(self + 0xd4);
    pos.y = *(int *)(self + 0xd8);
    pos.z = *(int *)(self + 0xdc);
    pos.y += 0x32000;

    if (DecIfAbove0_Byte((u8 *)(self + 0xe0)) != 0) {
        _ZN12CylinderClsn5ClearEv(self + 0xe4);
        if (*(u8 *)(self + 0xe0) == 1) {
            ((int *)&dust)[0] = ((int *)&pos)[0];
            ((int *)&dust)[1] = ((int *)&pos)[1];
            ((int *)&dust)[2] = ((int *)&pos)[2];
            _ZN5Actor19DisappearPoofDustAtERK7Vector3(self, &dust);
        }
        return 1;
    }

    *(int *)(self + 0x5c) = *(int *)(self + 0xd4);
    *(int *)(self + 0x60) = *(int *)(self + 0xd8);
    *(int *)(self + 0x64) = *(int *)(self + 0xdc);
    *(int *)((int)(self + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) &= ~0xc0000;

    if ((int)(*(unsigned short *)(self + 0xc) == 0x13d) != 0)
        func_020228dc(pos.x, pos.y, pos.z);
    else
        func_0202293c(pos.x, pos.y, pos.z);

    if ((int)((*(int *)(self + 0xb0) & 0x20000) != 0) != 0) {
        *(u8 *)(self + 0xe0) = 0x1e;
        v2.x = *(int *)(self + 0xd4);
        v2.y = *(int *)(self + 0xd8);
        v2.z = *(int *)(self + 0xdc);
        v2.y -= 0x1b000;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x9f, ((int *)&v2)[0], v2.y, v2.z);
    }

    {
        int id = *(int *)(self + 0x108);
        if (id != 0) {
            char *r = _ZN5Actor10FindWithIDEj(id);
            if (r != 0) {
                if ((int)(*(unsigned short *)(r + 0xc) == 0xbf) != 0) {
                    if ((*(int *)(self + 0x104) & 0x8000) == 0) {
                        _ZN6Player4BurnEv(r);
                    }
                }
            }
        }
    }

    _ZN12CylinderClsn5ClearEv(self + 0xe4);
    _ZN12CylinderClsn6UpdateEv(self + 0xe4);
    return 1;
}
