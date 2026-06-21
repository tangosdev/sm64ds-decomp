/* func_ov002_020f6c34 at 0x020f6c34
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int func_ov002_020f6514(void* a, void* b, int c);
extern unsigned char data_ov002_0210bc88;

int func_ov002_020f6c34(unsigned char* p, unsigned char* q) {
    unsigned char v = q[0];
    func_ov002_020f6514(*(void**)(p + 0xe0), &data_ov002_0210bc88, v);
    return 1;
}
