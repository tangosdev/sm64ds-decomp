// @symbol func_ov029_02111bcc
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjWc_Obj05_c.h"
// @emits daObjWc_Obj05_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjWc_Obj05_c::Behavior - recovered from vtable slot identity */
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern void func_020393a4(int* p, int v);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned a, unsigned b, unsigned c, void* pos, unsigned e);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(void* self);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* self, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);
extern void _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(void* self, void* mat, s16 s);

int daObjWc_Obj05_c_Behavior(u8* thiz)
{
    struct daObjWc_Obj05_c *self = (struct daObjWc_Obj05_c *)(void *)thiz;
    func_020393a4((int*)(thiz + 0x124), 0x250000);

    switch (thiz[0x32c]) {
    case 0:
        if (thiz[0x32a] != 0) {
            if (thiz[0x32b] == 0) {
                u8* p = (u8*)(((int)thiz + 0x32c) & 0xFFFFFFFFFFFFFFFFLL);
                *p = *p + 1;
            }
        }
        thiz[0x32b] = thiz[0x32a];
        break;
    case 1: {
        int* p60 = (int*)(((int)thiz + 0x60) & 0xFFFFFFFFFFFFFFFFLL);
        *p60 = *p60 - 0x14000;
        self->unk_324 = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(unsigned*)(thiz + 0x324), 3, 0x8d, thiz + 0x74, 0);
        {
            int v = self->unk_320 + (int)0xff5d8000;
            if (self->unk_060 <= v) {
                self->unk_060 = v;
                {
                    u8* p = (u8*)(((int)thiz + 0x32c) & 0xFFFFFFFFFFFFFFFFLL);
                    *p = *p + 1;
                }
                {
                    u8* base = (u8*)(((int)thiz + 0x300) & 0xFFFFFFFFFFFFFFFFLL);
                    *(u16*)(base + 0x28) = 0;
                }
            }
        }
        break;
    }
    case 2: {
        /* Rematerialize timer base - do not keep live across calls */
        if (*(u16*)((u8*)(((unsigned)thiz + 0x300)) + 0x28) >= 0x6e) {
            int snd = _ZN5Sound8PlayLongEjjjRK7Vector3j(
                *(unsigned*)(thiz + 0x324), 3, 0x8d, thiz + 0x74, 0);
            self->unk_324 = snd;
            {
                int* p60 = (int*)(((int)thiz + 0x60) & 0xFFFFFFFFFFFFFFFFLL);
                *p60 = *p60 + 0xa000;
            }
            if (self->unk_060 >= self->unk_320) {
                self->unk_060 = self->unk_320;
                thiz[0x32c] = 0;
                {
                    /* Different launder spelling so CSE does not reuse case-2 head base */
                    u8* b2 = (u8*)(((int)thiz + 0x300) & 0xFFFFFFFFFFFFFFFFLL);
                    *(u16*)(b2 + 0x28) = 0;
                }
            }
        } else {
            u16* p = (u16*)(((int)thiz + 0x328) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p + 1;
        }
        break;
    }
    case 3:
        break;
    }

    _ZN8Platform21UpdateModelPosAndRotYEv(thiz);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(thiz, 0, 0)) {
        _ZN8Platform19UpdateClsnPosAndRotEv(thiz);
    }
    _ZN18MovingMeshCollider9TransformERK9Matrix4x3s(
        thiz + 0x124, thiz + 0x2ec, self->unk_08e);
    thiz[0x32a] = 0;
    return 1;
}
