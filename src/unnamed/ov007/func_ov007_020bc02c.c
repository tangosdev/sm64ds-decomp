#include "types.h"
typedef struct { int f[12]; } Blk;

#define LS(a) (*(short*)(((long long)(int)(a))))

extern void func_ov007_020bc968(char *c, void *other);
extern void func_ov007_020bc894(char *c);
extern void _ZN15ModelComponents21UpdateVertsUsingBonesEv(void *self);
extern void _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(void *self, void *bca, int frame);
extern void func_ov007_020bc7dc(char *c);
extern void func_02044dc4(void *self);

void func_ov007_020bc02c(char *c, void *other) {
    void *r5 = *(void**)c;
    int r4 = 0;

    if (*(int*)(c + 0x9c) & 2) {
        char *r3 = *(char**)(c + 0x18);
        if (r3 != 0) {
            if (LS(r3 + 0xc) != -1) {
                if (*(int*)(r3) == 0) {
                    if (*(s16*)(r3 + 0xc) == -1)
                        goto set_r4;
                    {
                        int x = *(u16*)(r3 + 0x12);
                        int y = *(u16*)(r3 + 0x10);
                        if (y == x - 1)
                            goto skip_r4;
                    }
                set_r4:
                    r4 = 1;
                skip_r4:;
                }
            }
        }
    }

    func_ov007_020bc968(c, other);
    if (*(int*)(c + 0x98) != 0) {
        func_ov007_020bc894(c);
        _ZN15ModelComponents21UpdateVertsUsingBonesEv(r5);
    } else if (r4 != 0) {
        char *o = *(char**)(c + 0x18);
        char *bones = *(char**)(c + 0x14);
        _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(
            r5, *(void**)(bones + *(s16*)(o + 0xc) * 4), (u16)(*(u16*)(o + 0x10) + 1));
        *(int*)((char*)r5 + 0xc) = *(int*)(c + 8);
        _ZN15ModelComponents21UpdateVertsUsingBonesEv(r5);
        unsigned int i = 0;
        *(int*)((char*)r5 + 0xc) = *(int*)(c + 4);
        while (i < (unsigned int)(*(int*)(*(int*)r5 + 4))) {
            Blk *a = (Blk*)(*(char**)(c + 4) + i * 0x30);
            Blk *b = (Blk*)(*(char**)(c + 8) + i * 0x30);
            a->f[0] = (a->f[0] + b->f[0]) / 2;
            a->f[1] = (a->f[1] + b->f[1]) / 2;
            a->f[2] = (a->f[2] + b->f[2]) / 2;
            a->f[3] = (a->f[3] + b->f[3]) / 2;
            a->f[4] = (a->f[4] + b->f[4]) / 2;
            a->f[5] = (a->f[5] + b->f[5]) / 2;
            a->f[6] = (a->f[6] + b->f[6]) / 2;
            a->f[7] = (a->f[7] + b->f[7]) / 2;
            a->f[8] = (a->f[8] + b->f[8]) / 2;
            a->f[9] = (a->f[9] + b->f[9]) / 2;
            a->f[10] = (a->f[10] + b->f[10]) / 2;
            a->f[11] = (a->f[11] + b->f[11]) / 2;
            i++;
        }
    }

    if (*(int*)(c + 0x98) != 0 || r4 != 0) {
        if (*(s16*)(c + 0x8c) > 0)
            func_ov007_020bc7dc(c);
        if (*(int*)(*(int*)r5 + 0x30) != 0)
            func_02044dc4(r5);
    }

    {
        int flags = *(int*)(c + 0x9c);
        if ((flags & 1) == 0) {
            if ((flags & 2) == 0)
                return;
        }
        *(int*)(c + 0x98) = (*(int*)(c + 0x98) == 0) ? 1 : 0;
    }
}