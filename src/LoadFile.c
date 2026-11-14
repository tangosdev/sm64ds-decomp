/* LoadFile at 0x0201816c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

extern void *_Z13func_0201818cji(unsigned int handle, int mode);

void *LoadFile(int handle) {
    return _Z13func_0201818cji(handle, 1);
}
