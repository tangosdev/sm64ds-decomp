#include "types.h"
typedef struct { u8 pad[0x18]; } Rec18;

extern u8 data_020a0e40;
extern Rec18 data_0209f4ac[];
extern Rec18 data_0209f49c[];
extern Rec18 data_0209f4a0[];
extern int data_0209f32c;

extern int _ZNK6Player14GetBodyModelIDEjb(void *self, unsigned int a, int b);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void *self, unsigned int a, int b, Fix12i c, unsigned int d);
extern int _ZNK9Animation12WillHitFrameEi(void *anim, int frame);
extern u32 func_02022d00(u32 uniqueID, u32 effectID, Fix12i x, Fix12i y, Fix12i z, void *dir);
extern void func_0201251c(int a, int b, int c, int d);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, Fix12i a, Fix12i b, Fix12i c);

void func_ov002_020d1f78(void *selfPtr, u32 param)
{
    char *self = (char *)selfPtr;
    int idx;
    int flag;
    int id;
    char *anim;
    u32 w;
    int state;
    int target;

    idx = data_020a0e40;
    flag = 0;
    if (*(u8 *)&data_0209f4ac[idx] == 0) {
        if ((*(u16 *)&data_0209f49c[idx] & 0x800) == 0)
            flag = 1;
    } else {
        if (*(s16 *)&data_0209f4a0[idx] < 0xb33)
            flag = 1;
    }

    if (*(int *)(self + 8) == 2 && flag == 0) {
        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        anim = (char *)((long long)(((int *)(self + 0xdc))[id] + 0x50));
        w = *(u32 *)(anim + 8);
        _ZN6Player7SetAnimEji5Fix12IiEj(self, 0x3f, 0, 0x2800, (w << 4) >> 16);
    } else {
        int v = *(int *)(self + 0x98);
        int shift = v >> 3;
        if (shift < 0x400)
            shift = 0x400;
        _ZN6Player7SetAnimEji5Fix12IiEj(self, param, 0, shift, 0);
    }

    id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
    anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
    if (!_ZNK9Animation12WillHitFrameEi(anim, 4)) {
        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        anim = (char *)((int *)(self + 0xdc))[id] + 0x50;
        if (!_ZNK9Animation12WillHitFrameEi(anim, 0x13))
            return;
    }

    if (*(u8 *)(self + 0x707) != 0) {
        u32 newId = func_02022d00(*(u32 *)(self + 0x628), 0xde, *(int *)(self + 0x5c), data_0209f32c, *(int *)(self + 0x64), 0);
        *(u32 *)(self + 0x628) = newId;
        return;
    }

    func_0201251c(0, *(int *)(self + 0x66c) + 0x20, (int)(self + 0x74), *(int *)(self + 0x98));

    state = *(int *)(self + 0x66c);
    target = -1;
    if (state == 6)
        goto state_case6;
    if (state == 8)
        goto state_case8;
    if (state == 9) {
        target = 0xe6;
    }
    goto state_merge;
state_case8:
    target = 0xe4;
    goto state_merge;
state_case6:
    target = 0xe5;
state_merge:
    if (target == -1)
        return;

    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(target, *(int *)(self + 0x5c), *(int *)(self + 0x60), *(int *)(self + 0x64));
}
