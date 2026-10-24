/* storage 0x02384148 runtime 0x037FBFE0 size 0x7C (listed 0x78 + pool) wram ARM.
 * Scan 16 entries of 84 bytes at unk_38075C4; for each live entry (byte 1
 * clear) whose word-17 key lies in [a1, a2], clear bit 2 of byte 3 and notify
 * via fn_02383634(v4, aux). One pool, one call. The second call argument is a
 * constant-zero variable copy-initialized from v4 (MOV R9,R5, not MOV #0). */

extern int unk_38075C4[];
extern int fn_02383634(int a1, int a2);

unsigned fn_02384148(unsigned a1, unsigned a2) {
    unsigned v4 = 0;
    unsigned aux = 0;
    unsigned char *v6;
    unsigned v7;
    unsigned result;
    do {
        v6 = &((unsigned char *)unk_38075C4)[84 * v4];
        if (v6[1] == 0) {
            v7 = ((unsigned *)v6)[17];
            if (a1 <= v7 && v7 <= a2) {
                v6[3] &= ~2;
                fn_02383634((int)v4, (int)aux);
            }
        }
        result = v4 + 1;
        v4 = (unsigned char)(v4 + 1);
    } while (v4 < 0x10);
    return result;
}
