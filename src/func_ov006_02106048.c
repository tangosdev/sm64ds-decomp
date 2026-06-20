/* func_ov006_02106048 at 0x02106048
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_02106048(unsigned char *r0) {
    int i = 0;
    if (*(int *)(r0 + 0x4cb8) <= 0) return;
    do {
        (r0 + i + 0x4000)[0xf66] = 1;
        i++;
    } while (i < *(int *)(r0 + 0x4cb8));
}
