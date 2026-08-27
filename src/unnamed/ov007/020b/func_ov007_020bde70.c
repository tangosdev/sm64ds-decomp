/* func_ov007_020bde70 at 0x020bde70
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
 */

extern int* data_ov007_02104bbc;
extern void func_0204fa2c(int* p, int arg);

void func_ov007_020bde70(int idx, int arg) {
    if (data_ov007_02104bbc[idx] == 0) return;
    func_0204fa2c(&data_ov007_02104bbc[idx], arg);
}
