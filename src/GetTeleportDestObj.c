/* GetTeleportDestObj at 0x0202b07c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */

extern int *data_0209f330;

int *GetTeleportDestObj(int idx) {
    return data_0209f330 + idx * 2;
}
