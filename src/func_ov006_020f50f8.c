/* func_ov006_020f50f8 at 0x020f50f8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

struct Entry { int off0; int off4; };
extern struct Entry data_ov006_021422bc[];

void func_ov006_020f50f8(char *r0) {
    int idx = *(int *)(r0 + 0x5318);
    struct Entry *e = &data_ov006_021422bc[idx];
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
