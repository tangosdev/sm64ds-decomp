typedef unsigned char u8;

typedef struct {
    unsigned int id;
    int val;
} AnimEntry;

typedef struct {
    int x, y, z;
} V3;

extern AnimEntry *data_ov002_0210a7a0[];

extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *self, unsigned int id, int val, int fix, unsigned int flags);
extern int _ZNK6Player14GetBodyModelIDEjb(void *self, unsigned int a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void *anim, int frame);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void *v);
extern int _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(unsigned int handle, unsigned int type, int x, int y, int z, void *vec, void *cb);
extern int _ZN6Player12FinishedAnimEv(void *self);

void func_ov002_020bcdf0(char *self)
{
    AnimEntry *base;
    AnimEntry cur;
    int id;
    char *anim;
    unsigned int frame;
    V3 vv[2];
    int x0, y0, z0;

    if (*(u8 *)(self + 0x727) == 0)
        return;

    base = data_ov002_0210a7a0[*(u8 *)(self + 0x727) - 1];
    cur = base[*(u8 *)(self + 0x728)];

    _ZN6Player7SetAnimEji5Fix12IiEj(self, cur.id, cur.val, 0x1000, 0);

    if (cur.id == 0xbd) {
        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0xc)) {
            *(u8 *)(self + 0x71a) = 1;
        }
    } else if (cur.id == 1) {
        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
        if (_ZNK9Animation12WillHitFrameEi(anim, 0x5d)) {
            *(u8 *)(self + 0x71a) = 0;
        } else {
            id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
            anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
            if (_ZNK9Animation12WillHitFrameEi(anim, 0x45)) {
                x0 = *(int *)(self + 0x5c);
                y0 = *(int *)(self + 0x60) + 0x50000;
                z0 = *(int *)(self + 0x64);
                vv[0].x = x0;
                vv[0].y = y0;
                vv[0].z = z0;
                _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x37, vv[0].x, vv[0].y, vv[0].z);

                _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(self + 0x6d9), 0x33, self + 0x74);
            }
        }
    }

    if (*(int *)(self + 8) == 0) {
        if (cur.id == 0xbb) {
            id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
            anim = (char *)(((long long)(int)((char *)((int *)(self + 0xdc))[id] + 0x50)) & 0xffffffffffffffffLL);
            frame = (unsigned int)(*(int *)(anim + 8) << 4) >> 0x10;
            if (frame >= 0x1e)
                goto particle;
        }
        if (cur.id == 0xbc) {
particle:
            z0 = *(int *)(self + 0x64);
            y0 = *(int *)(self + 0x60) + 0x4b000;
            x0 = *(int *)(self + 0x5c);
            vv[1].x = x0;
            vv[1].y = y0;
            vv[1].z = z0;
            *(int *)(self + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(int *)(self + 0x628), 0x36, *(volatile int *)&vv[1].x, vv[1].y, vv[1].z, 0, 0);
        }
    }

    if (*(char **)(self + 0x72c) != 0) {
        char *p2 = *(char **)(self + 0x72c);
        *(short *)(p2 + 0x8c) = *(short *)(self + 0x8c);
        *(short *)(p2 + 0x8e) = *(short *)(self + 0x8e);
        *(short *)(p2 + 0x90) = *(short *)(self + 0x90);
        p2 = *(char **)(self + 0x72c);
        *(int *)(p2 + 0x5c) = *(int *)(self + 0x5c);
        *(int *)(p2 + 0x60) = *(int *)(self + 0x60);
        *(int *)(p2 + 0x64) = *(int *)(self + 0x64);
    }

    if (!_ZN6Player12FinishedAnimEv(self))
        return;

    cur.id = base[*(u8 *)(self + 0x728) + 1].id;
    cur.val = base[*(u8 *)(self + 0x728) + 1].val;
    if (cur.id != 0) {
        *(u8 *)(((long long)(int)(self + 0x728)) & 0xffffffffffffffffLL) += 1;
    }
}
