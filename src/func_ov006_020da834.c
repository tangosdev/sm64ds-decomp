/* func_ov006_020da834 at 0x020da834
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

struct S { char pad[0x24]; int v; };

int func_ov006_020da834(struct S *r0) {
    int i = 0;
    do {
        if (r0->v < 0x4000) return 0;
        i++;
        r0 = (struct S *)((char *)r0 + 0x30);
    } while (i < 5);
    return 1;
}
