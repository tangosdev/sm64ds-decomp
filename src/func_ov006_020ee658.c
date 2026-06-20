/* func_ov006_020ee658 at 0x020ee658
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

struct Pair { int a, b; };
extern struct Pair data_ov006_0213cb54;

void func_ov006_020ee658(unsigned char *r0) {
    r0 += 0x5000;
    *(unsigned short *)(r0 + 0x14) = 0x78;
    *(int *)(r0 + 0xc) = 0;
    *(int *)(r0 + 0x10) = 0x4000;
    *(struct Pair *)(r0 + 4) = data_ov006_0213cb54;
}
