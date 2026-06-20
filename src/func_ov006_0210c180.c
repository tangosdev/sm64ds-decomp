/* func_ov006_0210c180 at 0x0210c180
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

extern unsigned char data_0209d454;

void func_ov006_0210c180(int *r0) {
    r0[0] = 0x48;
    *(unsigned char *)((char *)r0 + 4) = 1;
    data_0209d454 |= 2;
}
