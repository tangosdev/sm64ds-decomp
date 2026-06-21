/* func_ov006_020dc298 at 0x020dc298
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

struct Entry { int off0; int off4; };
extern struct Entry data_ov006_021417c8[];

void func_ov006_020dc298(char *r0) {
    if (*(unsigned char *)(r0 + 0x51bc) == 0) return;
    unsigned char sel = *(unsigned char *)(r0 + 0x51bf);
    struct Entry *e = &data_ov006_021417c8[sel];
    int adj = e->off4;
    char *self = r0 + (adj >> 1);
    void (*fn)(char *, int);
    if (adj & 1) {
        fn = *(void (**)(char *, int))(*(char **)self + e->off0);
    } else {
        fn = (void (*)(char *, int))e->off0;
    }
    fn(self, 0);
}
