/* func_0204f9c4 at 0x0204f9c4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
typedef unsigned char u8;

extern void func_0204f5a0(u8 *thiz, int arg1);

extern u8 data_020a4d6c[];

struct S {
    char pad0[4];
    u8 *ptr;
    char pad8[0x24];
    u8 active;
    char pad2d[0x17];
};

extern struct S data_020a50ec[16];

void func_0204f9c4(int idx, int arg1) {
    int i;
    struct S *p = data_020a50ec;
    u8 *thiz = &data_020a4d6c[idx * 0x1c];
    for (i = 0; i < 16; i++, p++) {
        if (p->active != 0 && p->ptr == thiz) {
            func_0204f5a0((u8 *)p, arg1);
        }
    }
}
