/* func_ov006_020f1e58 at 0x020f1e58
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

extern unsigned char data_ov006_0213ceac[];

void func_ov006_020f1e58(unsigned char *r0, int r1) {
    unsigned short idx = *(unsigned short *)(r0 + 0x5174);
    r0 += r1;
    r0[0x53dd] = 1;
    r0[0x5275] = 0;
    r0[0x51fd] = data_ov006_0213ceac[idx] + 1;
}
