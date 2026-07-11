typedef unsigned char u8;
typedef unsigned short u16;

typedef struct { u16 flags; char pad[22]; } Entry24;

extern u8 data_020a0e40;
extern Entry24 data_0209f49c[];
extern u8 data_0209f2d8;
extern char data_0209f2f8;
extern void *data_0209f318;

extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int x, int y, int z);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *thiz, unsigned int id, int flags, int speed, unsigned int extra);
extern int _ZNK6Player14GetBodyModelIDEjb(void *thiz, unsigned int a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void *thiz, int f);
extern void func_ov002_020d4c30(void *self);

int func_ov002_020d2fdc(char *self)
{
    int r5;
    int id;
    char *anim;

    *(u8 *)(self + 0x6ed) = 0;
    if (*(u8 *)(self + 0x703) != 0) {
        return 0;
    }

    if ((unsigned short)((*(u16 *)(self + 0x6ce)) & 0x800)) {
        return 1;
    }

    if (data_0209f49c[data_020a0e40].flags & 0x800) {
        if (*(u8 *)(self + 0x6e5) < 0x1e) {
            *(u8 *)(((int)self + 0x6e5) & 0xFFFFFFFFFFFFFFFFLL) += 1;
            r5 = *(u8 *)(self + 0x6e5) * 0x111 + 0x1000;
        } else {
            *(u8 *)(self + 0x6ed) = 0x1e;
            r5 = 0x3000;
            if (*(u8 *)(self + 0x707) == 0) {
                _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(int *)(self + 0x5c), *(int *)(self + 0x60), *(int *)(self + 0x64));
            }
        }

        _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x3f, 0, r5, 0);

        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 4) ||
            (id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0),
             anim = (char *)((int *)(self + 0xdc))[id] + 0x50,
             _ZNK9Animation12WillHitFrameEi(anim, 0x13))) {
            func_ov002_020d4c30(self);
        }
        return 0;
    }

    *(u8 *)(self + 0x6e5) = 0;
    {
        int flag = 0;
        if (data_0209f2d8 == 1) flag = 1;
        if (flag != 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x47, 0, 0x1000, 0);
            return 0;
        }
    }

    if (*(int *)(self + 0x68c) > 0x32000) {
        _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x7d, 0, 0x1000, 0);
        return 0;
    }

    if (*(u8 *)(self + 0x6e3) == 0) {
        _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x47, 0, 0x1000, 0);
        if (*(u16 *)(self + 0x6a4) == 0) {
            char v = data_0209f2f8;
            if (v == 0x13 || v == 0xa || (unsigned)(v - 0x30) <= 1) {
                _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x95, 0x40000000, 0x1000, 0);
                *(u8 *)(self + 0x6e3) = 5;
            } else {
                *(u16 *)(self + 0x6a4) = 0x708;
                _ZN6Player7SetAnimEji5Fix12IiEj(self, 0xd, 0x40000000, 0x1000, 0);
                *(u8 *)(self + 0x6e3) = 1;
            }
            *(int *)(((int)(char *)data_0209f318 + 0x154) & 0xFFFFFFFFFFFFFFFFLL) |= 0x2000;
        }
    } else {
        _ZN6Player7SetAnimEji5Fix12IiEj(self, 0xf, 0, 0x1000, 0);
    }
    return 0;
}
