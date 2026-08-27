extern void func_ov007_020bdeb0(int a);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(void *p, unsigned short a, short b);
extern void func_ov007_020c86a8(void *c, int v);
extern void func_ov007_020c8688(void *p, int a);

union Unk30 {
    int word;
    struct {
        unsigned char b0;
        unsigned char b1;
        unsigned char b2;
        unsigned char b3;
    } b;
};

struct Obj6 {
    int unk0;
    char _4[8];
    int unkC;
    int unk10;
    char _14[4];
    int unk18;
    char _1C[8];
    int unk24;
    int unk28;
    int unk2C;
    union Unk30 unk30;
};

struct Obj4Inner { char _0[0x28]; int unk28; int unk2C; };
struct Obj4 { void *unk0; struct Obj4Inner *unk4; };

struct Sub {
    struct Obj4 *unk0;
    char _4[0x3C];
    int f40;
    char _44[4];
    int f48;
    char _4c[0x48];
    int f94;
};

struct G {
    struct Obj6 *obj6;
    struct Sub *sub;
    int flags;
    int unkC;
    int f10;
};

extern struct G *data_ov007_02104bd8;

void func_ov007_020bf790(void)
{
    struct G *g = data_ov007_02104bd8;
    struct Obj6 *r6 = g->obj6;
    int r5 = g->flags;
    int state = g->f10;
    int r4 = 2;

    switch (state) {
    default:
        break;
    case 1:
        r6->unk0 = 4;
        r6->unkC = 1;
        r6->unk10 = 0;
        r6->unk28 = 0x3F;
        r6->unk18 = 0x3C;
        r6->unk2C = 0x10;
        r6->unk30.word = 0;
        break;
    case 2:
        r6->unk0 = 4;
        r6->unkC = 1;
        r6->unk10 = 0;
        r6->unk28 = 0x3F;
        r6->unk18 = 0x3C;
        r6->unk2C = -0x10;
        r6->unk30.word = 0;
        break;
    case 3:
        r6->unk0 = 4;
        r6->unkC = 0;
        r6->unk10 = 0;
        r6->unk28 = 0x2F;
        r6->unk18 = 0x3C;
        r6->unk2C = -0x10;
        r6->unk30.word = 0;
        break;
    case 4:
        r6->unk0 = 4;
        r6->unkC = 0;
        r6->unk10 = 0;
        r6->unk28 = 0x3F;
        r6->unk18 = 0x100;
        r6->unk2C = -0x10;
        r6->unk30.word = 0;
        break;
    case 5:
        r6->unk0 = 4;
        r6->unkC = 1;
        r6->unk10 = 0;
        r6->unk28 = 0x3F;
        r6->unk18 = 0x3C;
        r6->unk2C = 0;
        r6->unk30.word = 0;
        break;
    case 6:
        r6->unk0 = 4;
        r6->unkC = 1;
        r6->unk10 = 0;
        r6->unk28 = 0x3F;
        r6->unk18 = 0x1000;
        r6->unk2C = -0x10;
        r6->unk30.word = -0x10;
        break;
    case 7:
        r6->unk0 = 4;
        r6->unkC = 1;
        r6->unk10 = 0;
        r6->unk28 = 0x2F;
        r6->unk18 = 0x1000;
        r6->unk2C = -0x10;
        r6->unk30.word = -0x10;
        break;
    case 8:
        r6->unk0 = 4;
        r6->unkC = 1;
        r6->unk10 = 0;
        r6->unk28 = 0x3D;
        r6->unk18 = 0x100;
        r6->unk2C = 0;
        r6->unk30.word = -9;
        break;
    case 9:
        r6->unk0 = 3;
        r6->unkC = 1;
        r6->unk10 = 0;
        r6->unk18 = 0x190;
        r6->unk28 = 1;
        r6->unk2C = 0x14;
        r6->unk30.b.b0 = 0x10;
        r6->unk30.b.b2 = 0;
        r6->unk30.b.b1 = 0;
        r6->unk30.b.b3 = 0;
        break;
    }

    if (r5 & 1) {
        int v;
        struct Obj4 *a = data_ov007_02104bd8->sub->unk0;
        if (state == 1) {
            v = 0x7FFF;
        } else {
            v = 0;
        }
        v = v | (v << 0x10);
        a->unk4->unk28 = v;
        a->unk4->unk2C = v;
        data_ov007_02104bd8->sub->f94 = 1;
        if (r5 & 2) {
            func_ov007_020bdeb0(0x30);
        } else {
            func_ov007_020bdeb0(0x31);
        }
    } else {
        if (r5 & 4) {
            r4 = 1;
        } else if (!(r5 & 8)) {
            r4 |= 1;
        }
        data_ov007_02104bd8->sub->f94 = 0;
    }

    if (r5 & 0x10) {
        _ZN3G2x18SetBlendBrightnessEPVtts((void*)0x4000050, 0x3F, 0);
    }

    if (r5 & 2) {
        func_ov007_020c86a8(r6, r4);
    } else {
        func_ov007_020c8688(r6, r4);
    }

    data_ov007_02104bd8->obj6->unk24 = data_ov007_02104bd8->unkC;
}
