/* func_ov002_020eab8c at 0x020eab8c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void DeathTable_ClearBit(int id);

void func_ov002_020eab8c(char *r0) {
    char *r4 = r0;
    void *a = _ZN5Actor10FindWithIDEj(*(unsigned int*)(r4 + 0x1cc));
    if (a == 0) return;
    if (*(short*)((char*)a + 0xce) >= 0) return;
    DeathTable_ClearBit(*(short*)(r4 + 0x100 + 0xd6));
}
