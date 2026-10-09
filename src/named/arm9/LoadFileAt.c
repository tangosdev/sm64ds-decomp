/* LoadFileAt at 0x020182ac
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

extern int func_02018270(int handle, int dest, int size);

int LoadFileAt(int handle, int dest) {
    return func_02018270(handle, dest, -1);
}
