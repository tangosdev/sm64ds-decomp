/* func_ov002_020e7e24 at 0x020e7e24
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int _ZN5Actor13SpawnSoundObjEj(void *self, unsigned int id);

void func_ov002_020e7e24(char *r0) {
    char *r4 = r0;
    if (*(unsigned char*)(r4 + 0x49e) != 0xff) {
        return;
    }
    if (_ZN5Actor13SpawnSoundObjEj(r4, 6) != 0) {
        *(unsigned char*)(r4 + 0x49e) = 0x78;
    }
}
