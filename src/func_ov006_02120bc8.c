/* func_ov006_02120bc8 at 0x02120bc8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

extern void *data_ov006_02142f64;

void func_ov006_02120bc8(char *r0) {
    *(void **)r0 = data_ov006_02142f64;
    data_ov006_02142f64 = r0;

    char *r1 = *(char **)r0;
    int v = *(int *)(r0 + 8) - 0x10000;
    if (r1 == 0) {
        return;
    }
    do {
        *(int *)(r1 + 0x10) = v;
        r1 = *(char **)r1;
        v -= 0x10000;
    } while (r1 != 0);
}
