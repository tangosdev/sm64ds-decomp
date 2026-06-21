/* func_ov002_020ecf94 at 0x020ecf94
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int func_ov002_020ec654(void *r0);
extern void func_ov002_020ecb0c(void *r0);
extern void func_ov002_020ecd18(void *r0);

void func_ov002_020ecf94(void *r0) {
    if (func_ov002_020ec654(r0)) {
        func_ov002_020ecb0c(r0);
    } else {
        func_ov002_020ecd18(r0);
    }
}
