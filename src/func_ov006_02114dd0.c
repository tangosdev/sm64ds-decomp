/* func_ov006_02114dd0 at 0x02114dd0
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_02114dd0(char *r0) {
    int i = 0;
    do {
        *(unsigned char *)(r0 + 0x5000 + 0x9bc) = 0;
        *(int *)(r0 + 0x5000 + 0x9b8) = 0;
        i++;
        *(int *)(r0 + 0x5000 + 0x9b4) = 0;
        r0 += 0x24;
    } while (i < 0x40);
}
