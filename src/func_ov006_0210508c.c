/* func_ov006_0210508c at 0x0210508c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_0210508c(char *r0) {
    r0 = r0 + 0x4000;
    *(int *)(r0 + 0x660) = 0x110000;
    *(int *)(r0 + 0x664) = 0x60000;
    *(int *)(r0 + 0x668) = -0x3000;
    r0[0x676] = 1;
    r0[0x675] = 1;
}
