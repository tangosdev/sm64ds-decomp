/* func_ov002_020f63d4 at 0x020f63d4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int func_ov002_020f5fb8(void* c);
extern int func_ov002_020f5f8c(void* c);

int func_ov002_020f63d4(unsigned char* p) {
    func_ov002_020f5fb8(*(void**)(p + 0xd8));
    func_ov002_020f5f8c(*(void**)(p + 0xd8));
    return 1;
}
