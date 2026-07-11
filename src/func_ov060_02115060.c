/* func_ov060_02115060 at 0x02115060 (ov060), size 0x64
 * Compiler mwccarm 1.2/sp2p3 */
extern void func_ov060_021150d0(char *);
extern void func_ov060_021150c4(char *);

void func_ov060_02115060(char *c) {
    if (*(unsigned char *)(c + 0x415) == 0) {
        if (*(unsigned char *)(c + 0x416) == 0) {
            func_ov060_021150d0(c);
        } else {
            func_ov060_021150c4(c);
        }
        (*(unsigned char *)(int)(((long long)(int)(c + 0x415)) & 0xFFFFFFFFFFFFFFFFLL))++;
        return;
    }
    *(unsigned char *)(c + 0x415) = 0;
    *(int *)(c + 0x40c) = 0xe;
}
