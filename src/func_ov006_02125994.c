/* func_ov006_02125994 at 0x02125994
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_02125994(unsigned char *r0) {
    int i;
    for (i = 0; i < 0x20; i++) {
        *(int *)(r0 + 0xb000 + 0xa14) = 0;
        *(int *)(r0 + 0xb000 + 0xa18) = 0;
        *(int *)(r0 + 0xb000 + 0xa1c) = 0;
        *(int *)(r0 + 0xb000 + 0xa20) = 0;
        *(int *)(r0 + 0xb000 + 0xa24) = 0;
        *(int *)(r0 + 0xb000 + 0xa28) = 0;
        *(int *)(r0 + 0xb000 + 0xa2c) = 0;
        r0[0xb000 + 0xa34] = 0;
        *(int *)(r0 + 0xb000 + 0xa30) = 0;
        r0 += 0x24;
    }
}
