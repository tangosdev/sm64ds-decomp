/* Message-bank lookup for the ov007 front end.
 * The INF1 halfword at +0x0a is a stride in bits; the index is narrowed
 * to 16 bits before multiplication. Each entry starts with a DAT1 offset.
 * Keep the existing integer-address ABI while the overlapping callers are
 * reconstructed. The unused first argument is also present at call sites.
 */
extern unsigned short *data_ov007_02104c1c; /* INF1 section header */
extern char *data_ov007_02104c18;           /* INF1 entry payload */
extern int data_ov007_02104c24;             /* DAT1 payload address */

int func_ov007_020c94a0(int unused, unsigned int messageIndex) {
    unsigned short entryStrideBits = data_ov007_02104c1c[5];
    unsigned int entryBitOffset = (unsigned int)(entryStrideBits * (messageIndex & 0xffff));
    return data_ov007_02104c24 + *(int *)(data_ov007_02104c18 + (entryBitOffset >> 3));
}
