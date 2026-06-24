/* func_ov006_0211a5ec at 0x0211ab80
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

extern int data_ov006_0212ee7c[];

void func_ov006_0211a5ec(char *r0, int idx) {
    int n = idx * 0x24;
    char *base = r0 + 0x51b0;
    char *p = r0 + n;
    *(unsigned char *)(p + 0x51cd) = 1;
    unsigned char k = *(unsigned char *)(p + 0x51d2);
    *(int *)(base + n) = *(int *)(base + n) + data_ov006_0212ee7c[k];
    *(int *)(p + 0x51bc) = -0x4000;
    *(unsigned char *)(p + 0x51d1) = 1;
}
