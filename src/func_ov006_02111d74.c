/* func_ov006_02111d74 at 0x02111d74
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

int func_ov006_02111d74(char *r0) {
    if (*(unsigned char *)(r0 + 0x30) == 0)
        return 1;
    if (*(unsigned char *)(r0 + 0x3a) == 1)
        return 1;
    {
        int v = *(int *)(r0 + 0xc);
        if (v < -0xa0000)
            return 1;
        if (v < 0) {
            if (*(int *)(r0 + 8) < 0xd0000)
                return 1;
        }
    }
    return 0;
}
