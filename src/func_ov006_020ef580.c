/* func_ov006_020ef580 at 0x020ef580
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov006).
 */

typedef struct { int a, b; } Pair;
extern Pair data_ov006_0213cc9c;

void func_ov006_020ef580(char *r0) {
    *(unsigned short *)(r0 + 0x5a74) = 0x20;
    *(Pair *)(r0 + 0x5004) = data_ov006_0213cc9c;
}
