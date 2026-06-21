/* func_ov002_020e6a78 at 0x020e6a78
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern unsigned short Color_Interp(void *self, unsigned short a, unsigned short b, int t);
extern void func_020555a4(void *p);
extern unsigned short data_ov002_0210a760[];

void func_ov002_020e6a78(void *self, int arg)
{
    unsigned short *src = (unsigned short *)self;
    unsigned short *d = data_ov002_0210a760;
    unsigned short *dst = (unsigned short *)((char *)self + 0x40);
    int i;
    for (i = 0; i < 0x20; i++) {
        *dst++ = Color_Interp(self, *src++, *d++, arg);
    }
    func_020555a4((char *)self + 0x40);
}
