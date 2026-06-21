/* func_ov006_021050bc at 0x021050bc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

struct Entry { int a; int b; };
extern struct Entry data_ov006_02142860[];

void func_ov006_021050bc(char *r0) {
    if (*(unsigned char *)(r0 + 0x4674) == 0) return;
    unsigned char idx = *(unsigned char *)(r0 + 0x4675);
    struct Entry *e = &data_ov006_02142860[idx];
    int b = e->b;
    r0 = r0 + (b >> 1);
    void (*fn)(char *);
    if (b & 1) {
        fn = (void (*)(char *))(*(int *)(*(int *)r0 + e->a));
    } else {
        fn = (void (*)(char *))e->a;
    }
    fn(r0);
}
