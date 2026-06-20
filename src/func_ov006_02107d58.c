/* func_ov006_02107d58 at 0x02107d58
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

void func_ov006_02107d58(int *r0) {
    int i = 0;
    do {
        r0[i] = 0;
        i++;
    } while (i < 5);
    *(unsigned short *)((char *)r0 + 0x14) = 4;
    *(unsigned short *)((char *)r0 + 0x16) = 0;
}
