/* func_ov006_020e82fc at 0x020e82fc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

struct Entry { int off0; int off4; };
extern struct Entry data_ov006_02141f44[];

void func_ov006_020e82fc(char *r0) {
    unsigned char idx = *(unsigned char *)(r0 + 0x5552);
    if (idx >= 3) return;
    struct Entry *e = &data_ov006_02141f44[idx];
    int adj = e->off4;
    char *self = r0 + (adj >> 1);
    void (*fn)(char *);
    if (adj & 1) {
        fn = *(void (**)(char *))(*(char **)self + e->off0);
    } else {
        fn = (void (*)(char *))e->off0;
    }
    fn(self);
}
