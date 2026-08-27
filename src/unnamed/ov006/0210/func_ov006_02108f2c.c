#include "types.h"
enum { false = 0, true = 1 };

typedef struct Thing {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s16 unk2C;
    s16 unk2E;
    s16 unk30;
    u8 unk32;
    u8 unk33;
} Thing;

extern void _Z14ApproachLinearRiii(s32* v, s32 target, s32 step);
extern int func_ov006_02108e24(Thing* t);
extern void Vec2_Sub(s32* out, s32* a, Thing* b);
extern void func_ov004_020b1b40(int c);
extern void func_ov006_021094ac(Thing* t, s32* v);
extern s16 func_ov006_02108650(s32 x, s32 z);
extern void func_ov006_02109530(Thing* t, u8* p, s32 scale);
extern void func_ov006_0210935c(Thing* t, s32 idx);
extern void func_ov006_0210927c(Thing* t);
extern void _ZN5Sound12PlayBank2_2DEj(u32 id);

extern s32 data_ov004_020b9488;
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern u8 data_ov006_02142ab4[];
extern s32 data_ov006_021428c8;

void func_ov006_02108f2c(Thing* t)
{
    u8 state;
    int b;
    s16 type;

    state = t->unk32;
    if (state == 0 || state == 3) {
        _Z14ApproachLinearRiii(&t->unk0, t->unk18, t->unk10);
        _Z14ApproachLinearRiii(&t->unk4, t->unk1C, t->unk14);
    }

    state = t->unk32;
    if (state == 1) {
        func_ov006_02108e24(t);
        _Z14ApproachLinearRiii(&t->unk0, t->unk18, t->unk10);
        _Z14ApproachLinearRiii(&t->unk4, t->unk1C, t->unk14);
    } else if (state == 7) {
        s32 out[2];
        _Z14ApproachLinearRiii(&t->unk0, t->unk18, t->unk10);
        _Z14ApproachLinearRiii(&t->unk4, t->unk1C, t->unk14);
        Vec2_Sub(out, &t->unk18, t);
        if (out[0] != 0)
            return;
        if (out[1] != 0)
            return;
        t->unk32 = 6;
        func_ov004_020b1b40(1);
    } else if (state == 4) {
        s32 out[2];
        _Z14ApproachLinearRiii(&t->unk0, t->unk18, t->unk10);
        _Z14ApproachLinearRiii(&t->unk4, t->unk1C, t->unk14);
        Vec2_Sub(out, &t->unk18, t);
        if (out[0] != 0)
            return;
        if (out[1] == 0) {
            t->unk30 = 0x14;
            t->unk32 = 5;
        }
    } else if (state == 5) {
        s32 d;
        s32* p;
        s32 q;
        d = data_ov004_020b9488;
        p = (s32*)(((s32)t + 4));
        q = -((t->unk30 - (d >> 1)) << 12) / d - 0x600;
        *p += (s32)((((s64)q << 14) + 0x800) >> 12);
        *(s16*)(((s32)t + 0x30)) -= 1;
        if (t->unk30 == 0)
            t->unk32 = 6;
    } else if (state == 2) {
        s32 vec[2];
        u8 idx = data_020a0e40;
        if (data_020a0de8[idx * 4] != 0) {
            vec[0] = data_020a0dea[idx * 4] << 12;
            vec[1] = data_020a0deb[idx * 4] << 12;
            func_ov006_021094ac(t, vec);
        } else {
            type = func_ov006_02108650(t->unk18 >> 12, t->unk1C >> 12);
            t->unk2C = type;
            if (type != 0x25) {
                b = 0;
                type = t->unk2C;
                if (type >= 0xc && type <= 0x1c)
                    b = 1;
                if (b != false)
                    goto use_table;
                if (type >= 0x1d && type <= 0x22)
                    b = 1;
                else
                    b = 0;
                if (b == false)
                    goto no_table;
            use_table:
                func_ov006_02109530(t, &data_ov006_02142ab4[type * 8], 0x100);
                goto snd;
            no_table:
                func_ov006_0210935c(t, type);
            snd:
                _ZN5Sound12PlayBank2_2DEj(0x15e);
            } else {
                _ZN5Sound12PlayBank2_2DEj(0x15f);
                func_ov006_0210927c(t);
            }
            data_ov006_021428c8 = 0;
            t->unk32 = 1;
        }
        _Z14ApproachLinearRiii(&t->unk0, t->unk18, t->unk10);
        _Z14ApproachLinearRiii(&t->unk4, t->unk1C, t->unk14);
    }
}
