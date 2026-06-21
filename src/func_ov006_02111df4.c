/* func_ov006_02111df4 at 0x02111df4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

int func_ov006_02111df4(char *r0) {
    if ((*(int *)(*(char **)(r0 + 4) + 8) & 0xff) != 0
        && *(int *)(r0 + 8) < 0xe0000
        && *(int *)(r0 + 0xc) >= 0x50000
        && *(int *)(r0 + 0xc) < 0x88000
        && *(int *)(r0 + 0x11c) >= 0x78)
        return 1;
    if (*(unsigned char *)(r0 + 0x121) == 0)
        return 1;
    return *(unsigned char *)(r0 + 0x3b);
}
