/* func_ov006_020ef7f8 at 0x020ef7f8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

struct Pair { int a, b; };
extern struct Pair data_ov006_0213cc94;

void func_ov006_020ef7f8(unsigned char *r0) {
    *(unsigned short *)(r0 + 0x5a74) = 0x78;
    r0 += 0x5000;
    *(int *)(r0 + 0xa64) = 0;
    *(int *)(r0 + 0xa68) = 0x4000;
    *(struct Pair *)(r0 + 4) = data_ov006_0213cc94;
}
