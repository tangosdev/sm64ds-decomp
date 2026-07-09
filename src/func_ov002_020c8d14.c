typedef unsigned char u8;

struct V3 { int x, y, z; };

extern int _ZNK6Player14GetBodyModelIDEjb(void *thiz, unsigned int a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void *thiz, int f);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void func_ov002_020c9e18(char *self);
extern int func_ov002_020d22ec(char *self, int arg);
extern int _ZN6Player12FinishedAnimEv(char *self);
extern void _ZN6Player11ChangeStateERNS_5StateE(char *self, void *st);
extern int data_ov002_021106ac;
extern int data_ov002_0211013c;

int func_ov002_020c8d14(char *self)
{
    struct V3 v;
    int val;
    int id;
    char *anim;

    val = *(int *)(self + 0x84) + 0xf0;
    if (val >= 0x3000) val = 0x3000;
    *(int *)(self + 0x80) = val;
    *(int *)(self + 0x84) = val;
    *(int *)(self + 0x88) = val;

    v.x = *(int *)(self + 0x5c);
    v.y = *(int *)(self + 0x60);
    v.z = *(int *)(self + 0x64);
    v.y = *(int *)(self + 0x60) + (int)(((long long)*(int *)(self + 0x84) * 0x50000 + 0x800) >> 12);

    id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
    anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
    if (_ZNK9Animation12WillHitFrameEi(anim, 4)) {
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x5d, v.x, v.y, v.z);
    } else {
        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 2)) {
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x5e, v.x, v.y, *(volatile int *)&v.z);
        }
    }

    id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
    anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
    if (_ZNK9Animation12WillHitFrameEi(anim, 0x2f)) {
        func_ov002_020c9e18(self);
        *(unsigned short *)(self + 0x6c2) = 0x258;
    }

    id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
    anim = (char *)(((long long)(int)((char *)((int *)(self + 0xdc))[id] + 0x50)) & 0xffffffffffffffffLL);
    if ((unsigned int)(*(int *)(anim + 8) << 4) >> 0x10 >= 0x2f) {
        if (*(u8 *)(self + 0x706) == 0) {
            if (func_ov002_020d22ec(self, 0))
                return 0;
        }
    }

    if (_ZN6Player12FinishedAnimEv(self)) {
        if (*(u8 *)(self + 0x706) != 0)
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_021106ac);
        else
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211013c);
    }
    return 0;
}
