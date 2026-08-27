#include "types.h"
typedef struct {
    s32 unk0;
    u8 unk4, unk5, unk6, unk7, unk8, unk9, unkA, unkB;
    u16 unkC, unkE, unk10;
    u8 unk12[0x10];
    u8 unk22;
    u8 unk23;
} Rec;

struct Col4 { u16 a, b, c, d; };

extern Rec data_020a1040;
extern u8 data_020a0f04;
extern u8 data_020a0f10;
extern u8 data_02099e18;
extern u16 data_020a0f1c;
extern int data_020a0f98;

extern void func_0203b9bc(struct Col4 *out);
extern int func_0203ea5c(void);
extern void func_0203e20c(void);
extern void func_0203e0ac(void);
extern void func_02040724(void);

void func_0203df40(void)
{
    struct Col4 col;
    u8 sel;

    data_020a1040.unkE = ((*(volatile u16 *)0x4000130 | *(volatile u16 *)0x27fffa8) ^ 0x2fff) & 0x2fff;

    func_0203b9bc(&col);

    *(u16 *)&data_020a1040.unk4 = col.a;
    *(u16 *)&data_020a1040.unk6 = col.b;
    *(u16 *)&data_020a1040.unk8 = col.c;
    *(u16 *)&data_020a1040.unkA = col.d;
    data_020a1040.unkC |= 0x8000;

    sel = data_020a0f04;
    switch (sel) {
    case 1:
        func_0203ea5c();
        break;
    case 2:
        func_0203ea5c();
        break;
    case 3:
        func_0203e20c();
        break;
    default:
        data_020a0f04 = 0;
    case 0:
        func_0203e0ac();
        break;
    }

    data_020a1040.unk0 += 1;

    if (data_020a0f04 == 0) {
        switch (data_020a0f98) {
        default:
        case 0:
        case 3:
            break;
        case 1:
        case 2:
            data_020a0f1c &= 0x7fff;
            func_02040724();
            data_02099e18 = 1;
            data_020a0f10 = 0;
            break;
        }
    }

    data_020a0f98 = data_020a0f04;
}
