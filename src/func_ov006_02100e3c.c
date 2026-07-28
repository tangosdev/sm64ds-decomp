typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

struct Sub {
    /* 0x00 */ s32 unk00;
    /* 0x04 */ s32 unk04;
    /* 0x08 */ s32 unk08;
    /* 0x0c */ s32 unk0c;
    /* 0x10 */ s32 unk10;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
    /* 0x1c */ s32 unk1c;
    /* 0x20 */ s32 unk20;
    /* 0x24 */ s32 unk24;
    /* 0x28 */ s32 unk28;
    /* 0x2c */ s32 unk2c;
    /* 0x30 */ u16 unk30;
    /* 0x32 */ u16 unk32;
    /* 0x34 */ u16 unk34;
    /* 0x36 */ u8 unk36;
    /* 0x37 */ u8 unk37;
    /* 0x38 */ u8 unk38;
    /* 0x39 */ u8 unk39;
    /* 0x3a */ u8 unk3a;
    /* 0x3b */ u8 unk3b;
    /* 0x3c */ s32 unk3c;
};

struct Obj {
    /* 0x0000 */ u8 pad0[0x5260];
    /* 0x5260 */ struct Sub sub[2];
};

void func_ov006_02100e3c(struct Obj *obj, int idx)
{
    obj->sub[idx].unk00 += obj->sub[idx].unk08;

    if (obj->sub[idx].unk3a == 0) {
        obj->sub[idx].unk08 += 0x200;
        if (obj->sub[idx].unk08 >= 0x1800) {
            obj->sub[idx].unk3a = 1;
        }
    } else if (obj->sub[idx].unk3a == 1) {
        obj->sub[idx].unk08 -= 0x200;
        if (obj->sub[idx].unk08 <= -0x1800) {
            obj->sub[idx].unk3a = 0;
        }
    }

    if (obj->sub[idx].unk32 != 0) {
        obj->sub[idx].unk32--;
        return;
    }

    if ((obj->sub[idx].unk00 - obj->sub[idx].unk1c) >> 12 < -2) {
        return;
    }
    if ((obj->sub[idx].unk00 - obj->sub[idx].unk1c) >> 12 > 2) {
        return;
    }

    obj->sub[idx].unk32 = 0x10;
    obj->sub[idx].unk0c = 0;
    obj->sub[idx].unk36 = 0xb;
    obj->sub[idx].unk00 = obj->sub[idx].unk1c;
    obj->sub[idx].unk08 = 0;
}
