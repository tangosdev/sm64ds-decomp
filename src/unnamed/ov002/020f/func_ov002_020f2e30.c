/* func_ov002_020f2e30 at 0x020f2e30
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern void func_0201f108(void);
extern void func_0201ef50(unsigned int x);
extern unsigned char data_0209d454;

struct Obj {
    char pad1c4[0x1c4];
    unsigned short f1c4;   /* +0x1c4 */
    unsigned short f1c6;   /* +0x1c6 */
    unsigned char f1c8;    /* +0x1c8 */
    unsigned char f1c9;    /* +0x1c9 */
    unsigned char f1ca;    /* +0x1ca */
    unsigned char f1cb;    /* +0x1cb */
};

void func_ov002_020f2e30(struct Obj *self, int n) {
    if (n == 0) return;
    if (n == 0x15) return;
    self->f1c8 = 1;
    self->f1c9 = 0;
    self->f1c4 = 0;
    self->f1c6 = 0;
    self->f1ca = 0;
    n = (n - 1) << 1;
    self->f1cb = (unsigned char)n;
    func_0201f108();
    func_0201ef50(n & 0xff);
    data_0209d454 |= 1;
}
