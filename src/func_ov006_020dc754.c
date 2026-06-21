/* func_ov006_020dc754 at 0x020dc754
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

struct Entry {
    /* 0x0 */ int off0;
    /* 0x4 */ int field4;
};

extern struct Entry data_ov006_021417e8[];

void func_ov006_020dc754(char *r0) {
    if (*(unsigned char *)(r0 + 0x5000 + 0x1a0) == 0) {
        return;
    }
    struct Entry *e = &data_ov006_021417e8[*(unsigned char *)(r0 + 0x5000 + 0x1a1)];
    int f4 = e->field4;
    int r2;
    r0 += f4 >> 1;
    if (f4 & 1) {
        r2 = *(int *)(*(int *)r0 + e->off0);
    } else {
        r2 = e->off0;
    }
    ((void (*)(char *, int))r2)(r0, 0);
}
