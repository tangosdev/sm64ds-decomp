#include "types.h"
extern void _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(u32, int);
extern int _ZN6Player12GetTalkStateEv(void*);
extern void _ZN7Message13DisplaySavingEt(u16);
extern void _ZN7Message7EndTalkEv(void);
extern int func_ov002_020c6e14(void*);
extern void func_ov002_020e8618(char*);

extern u8 data_0209d660;

typedef struct Sub {
    u8 pad[0x8e];
    s16 x8e;
} Sub;

typedef struct Obj {
    u8 pad0[0x94];
    s16 x94;
    u8 pad1[0x438 - 0x96];
    u8* x438;
    int x43c;
    u8 pad2[0x49b - 0x440];
    u8 x49b;
    u8 pad3[0x4a1 - 0x49c];
    u8 x4a1;
    u16 x4a2;
} Obj;

void func_ov002_020e9af4(Obj* self)
{
    if (self->x43c == 9) {
        if (self->x4a1 < 0x78) {
            _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0, 0x7f000);
            (*(u8*)((int)self + 0x4a1))++;
        } else {
            _ZN5Sound17ChangeMusicVolumeEj5Fix12IiE(0x40, 0xcb33);
        }
    }
    self->x94 = ((Sub*)self->x438)->x8e;
    switch (self->x49b) {
    case 2:
        if (_ZN6Player12GetTalkStateEv(self->x438) == -1) {
            u32 t = ((u32)self->x4a2 << 20) >> 30;
            if (t == 1) {
                _ZN7Message13DisplaySavingEt(0x295);
                (*(u8*)((int)self + 0x49b))++;
                {
                    u16* p = (u16*)((int)self->x438 + 0x6ce);
                    *p = *p | 0x800;
                }
            } else if (t == 2) {
                (*(u8*)((int)self + 0x49b)) += 2;
                _ZN7Message7EndTalkEv();
            }
        }
        break;
    case 3:
        if (data_0209d660 == 0) {
            (*(u8*)((int)self + 0x49b))++;
        }
        break;
    case 4:
        if (func_ov002_020c6e14(self->x438) != 0) {
            (*(u8*)((int)self + 0x49b))++;
        } else {
            (*(u8*)((int)self + 0x49b)) += 2;
        }
        break;
    case 5:
        if (data_0209d660 == 0) {
            _ZN7Message7EndTalkEv();
            (*(u8*)((int)self + 0x49b))++;
        }
        break;
    case 6:
        {
            u16* p = (u16*)((int)self->x438 + 0x6ce);
            *p = *p & ~0x800;
        }
        {
            u16* q = (u16*)((int)self + 0x4a2);
            *q = *q & ~2;
        }
        func_ov002_020e8618((char*)self);
        break;
    }
}
