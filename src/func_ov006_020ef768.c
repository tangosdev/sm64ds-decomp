/* func_ov006_020ef768 at 0x020ef768
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

struct Pair { int a, b; };
extern struct Pair data_ov006_0213cc84;

void func_ov006_020ef768(unsigned char *r0) {
    *(unsigned short *)(r0 + 0x5a74) = 0xf0;
    *(struct Pair *)(r0 + 0x5004) = data_ov006_0213cc84;
}
