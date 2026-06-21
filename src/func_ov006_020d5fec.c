/* func_ov006_020d5fec at 0x020d5fec
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

struct Entry { int off0; int off4; };
extern struct Entry data_ov006_02141660[];

void func_ov006_020d5fec(char *r0) {
    if (*(unsigned char *)(r0 + 0x62ad) == 0) return;
    unsigned char idx = *(unsigned char *)(r0 + 0x62ac);
    struct Entry *e = &data_ov006_02141660[idx];
    int adj = e->off4;
    char *self = r0 + (adj >> 1);
    void (*fn)(char *, int);
    if (adj & 1) {
        fn = *(void (**)(char *, int))((char *)*(int *)self + e->off0);
    } else {
        fn = (void (*)(char *, int))e->off0;
    }
    fn(self, 0);
}
