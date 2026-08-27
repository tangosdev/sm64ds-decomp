/* func_ov002_020c9718 at 0x020c9718
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int func_ov002_020ceaf4(unsigned char *self);
extern int data_0209f32c;

void func_ov002_020c9718(unsigned char *self)
{
    int r2;
    if (self[0x706] == 0)
        return;
    *(int *)(self + 0xa8) = func_ov002_020ceaf4(self);
    r2 = data_0209f32c - 0x50000;
    if (*(int *)(self + 0x60) < r2)
        return;
    if (*(int *)(self + 0xa8) >= 0)
        *(int *)(self + 0x60) = r2;
}
