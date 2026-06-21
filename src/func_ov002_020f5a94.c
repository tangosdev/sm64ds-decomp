/* func_ov002_020f5a94 at 0x020f5a94
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int _ZN8SaveData19IsCharacterUnlockedEj(unsigned int c);

int func_ov002_020f5a94(void) {
    unsigned char count = 0;
    int i = 0;
    do {
        if (_ZN8SaveData19IsCharacterUnlockedEj(i)) {
            count = count + 1;
        }
        i++;
    } while (i < 4);
    return count;
}
