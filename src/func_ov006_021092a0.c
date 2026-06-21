/* func_ov006_021092a0 at 0x021092a0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

struct S {
    /* 0x00 */ int a;
    /* 0x04 */ int b;
    /* 0x08 */ char pad8[0x10 - 0x08];
    /* 0x10 */ int f10;
    /* 0x14 */ int f14;
    /* 0x18 */ int f18;
    /* 0x1c */ int f1c;
    /* 0x20 */ char pad20[0x28 - 0x20];
    /* 0x28 */ int f28;
    /* 0x2c */ short f2c;
    /* 0x2e */ short f2e;
    /* 0x30 */ char pad30[0x32 - 0x30];
    /* 0x32 */ unsigned char f32;
};

void func_ov006_021092a0(struct S *r0) {
    r0->a = 0xc000;
    r0->b = 0xc000;
    r0->f18 = r0->a;
    r0->f1c = r0->b;
    r0->f10 = 0;
    r0->f14 = 0;
    r0->f32 = 6;
    r0->f2c = 0x25;
    r0->f2e = 0;
    r0->f28 = 1;
}
