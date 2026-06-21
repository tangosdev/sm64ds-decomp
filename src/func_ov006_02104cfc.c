/* func_ov006_02104cfc at 0x02104cfc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_02104cfc(char *r0, int idx) {
    *(unsigned char *)(r0 + 0x4684) = 1;
    *(unsigned char *)(r0 + 0x4685) = 1;
    *(int *)(r0 + 0x4678) = *(int *)(r0 + idx * 4 + 0x4cc4);
    *(int *)(r0 + 0x467c) = *(int *)(r0 + idx * 4 + 0x4d54) + 0x8000;
    *(short *)(r0 + 0x4680) = 0x30;
    *(short *)(r0 + 0x4682) = 0;
    *(unsigned char *)(r0 + 0x4686) = 0;
}
