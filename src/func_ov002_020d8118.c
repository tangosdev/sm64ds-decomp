/* func_ov002_020d8118 at 0x020d8118
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void func_ov002_020bda48(void *self);
extern void func_ov002_020bd9ec(char *c, unsigned int r4);
extern void func_ov002_020c43c4(void *self, int a);

void func_ov002_020d8118(char *r0) {
    char *r4 = r0;
    func_ov002_020bda48(r4);
    *(unsigned short*)(r4 + 0x600 + 0xbe) = 0x258;
    func_ov002_020bd9ec(r4, 0x32);
    func_ov002_020c43c4(r4, 5);
    *(unsigned char*)(r4 + 0x6f8) = 1;
}
