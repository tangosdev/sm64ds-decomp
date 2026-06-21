/* func_ov002_020f37fc at 0x020f37fc
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void func_ov002_020f3828(void *this);

void func_ov002_020f37fc(void *this) {
    func_ov002_020f3828(this);
    *((unsigned char *)this + 0x15c) = 1;
    *((unsigned char *)this + 0x15d) = 2;
    *((unsigned char *)this + 0x15e) = 0;
}
