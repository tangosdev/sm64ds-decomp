/* func_ov006_020f82d0 at 0x020f82d0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

struct S {
    /* 0x00 */ char pad0[0x28];
    /* 0x28 */ short f28;
    /* 0x2a */ char pad[0x2d - 0x2a];
    /* 0x2d */ unsigned char f2d;
};

void func_ov006_020f82d0(struct S *r0, int r1) {
    r0->f28 = (4 - (r1 % 5)) * 2;
    r0->f2d = 5;
}
