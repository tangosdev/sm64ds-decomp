#include "types.h"
extern void *Actor_FindWithID(unsigned int id);
extern int func_ov020_021115ac(void *thiz);
extern void Sound_PlayBank0(unsigned int id, void *pos);
extern void func_ov020_02111340(void *thiz);
extern void func_ov063_0211cae8(void *found, unsigned int mask);
extern void ActorBase_MarkForDestruction(void *thiz);
extern u8 data_ov020_02114828[];

#define LDR(p) ((int)((((long long)(int)(p)))))

void func_ov020_0211174c(char *c)
{
    char *found;

    Actor_FindWithID(*(unsigned int *)(c + 0x418));
    found = (char *)Actor_FindWithID(*(unsigned int *)(c + 0x418));
    if (found == 0)
        return;

    switch (*(s32 *)(c + 0x424)) {
    case 6:
        if ((*(u8 *)(found + 0x157) & 8) == 0)
            return;
        *(s32 *)(c + 0x424) = 7;
        *(u16 *)(c + 0x100) = 0;
        return;
    case 7:
    {
        u16 *ctr = (u16 *)(c + 0x100);
        s32 *z;
        s32 *fl;
        if (*ctr != 0) {
            *ctr -= 1;
            return;
        }
        z = (s32 *)LDR(c + 0x64);
        *z -= 0x4000;
        if (*(s32 *)(c + 0x434) - *(s32 *)(c + 0x64) < 0x28000)
            return;
        *(s32 *)(c + 0x64) = *(s32 *)(c + 0x434) - 0x28000;
        fl = (s32 *)LDR(c + 0x234);
        *(s32 *)(c + 0x424) = 8;
        *fl &= ~1;
        return;
    }
    case 8:
    {
        s32 *fl;
        if (func_ov020_021115ac(c) <= 0)
            return;
        fl = (s32 *)LDR(c + 0x234);
        *(s32 *)(c + 0x424) = 9;
        *fl |= 1;
        Sound_PlayBank0(0xb5, c + 0x74);
        return;
    }
    case 9:
    {
        s32 *z = (s32 *)LDR(c + 0x64);
        *z += 0xa000;
        if (*(s32 *)(c + 0x64) < *(s32 *)(c + 0x434))
            return;
        {
            int idx = *(s32 *)(c + 8);
            if ((*(u8 *)(found + 0x157) & 7) != data_ov020_02114828[idx])
                func_ov020_02111340(c);
        }
        *(s32 *)(c + 0x41c) = 0;
        func_ov063_0211cae8(found, (1u << *(s32 *)(c + 8)) & 0xff);
        *(s32 *)(c + 0x64) = *(s32 *)(c + 0x434);
        *(s32 *)(c + 0x424) = 0xa;
        return;
    }
    case 10:
    {
        s32 *z;
        if ((*(u8 *)(found + 0x157) & (1 << *(s32 *)(c + 8))) == 0) {
            *(s32 *)(c + 0x424) = 7;
            *(u16 *)(c + 0x100) = 0xa;
            return;
        }
        if (*(u8 *)(found + 0x150) == 2) {
            z = (s32 *)LDR(c + 0x5c);
            *z += 0x5000;
            if (*(s32 *)(c + 0x5c) < (s32)0xffa24000)
                return;
            ActorBase_MarkForDestruction(c);
            return;
        }
        if (*(u8 *)(found + 0x150) != 3)
            return;
        ActorBase_MarkForDestruction(c);
        return;
    }
    default:
        return;
    }
}
