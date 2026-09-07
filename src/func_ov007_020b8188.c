/* Minigame results / record-board text layout (ov007 0x020b8188, 0x360 bytes).
 * Eighteen rows: row 0 prints the title (func_ov007_020b8a78) and the entry
 * count at p1[sl + 2]; row 1 prints one of two heading strings (0x1f57, then
 * 0x1f62 or 0x1f58 depending on p2); row 0x11 prints string 0x13b0 and the
 * byte at *(int *)p1 + sl + 0x98; rows 2..0x10 print one record each from the
 * 0x1388 string table plus three bytes of the record at
 * *(int *)p1 + sl * 0xf + row - 2 (the first clamped to 7 when it reaches 8).
 * data_ov007_02103464 is the column, data_ov007_02103468 the row pitch (2 per
 * row), data_ov007_02102dd8 the font.
 *
 * Every literal below is a named local because the ROM keeps them all in the
 * frame (0x1c..0x4c, one slot per constant, in declaration order) and reloads
 * them at each use; the register set is the counter, the parameter, five
 * register constants and the column pointer. Shapes that were load-bearing:
 * `zero` is a volatile local assigned right before the call (the corpus idiom
 * for MultiStore32Bytes's value argument); `sb = 0; s14 = 0;` precede the
 * loop so the zero-valued slots store from the counter register; base15 is
 * assigned AFTER the call (a declaration initializer computes it before);
 * the clamp is a ternary, not an if, which raises s48's web priority so its
 * spill store lands after s3c's; row/m use `+=` for the ROM's add operand
 * order; the row-1 heading is an if/else inside case 1 (the 0x1f58 chain is
 * the else arm, not part of case 0x11). */
typedef unsigned char u8;
typedef short s16;

extern int data_ov007_02103478[];
extern int data_ov007_020d7a5c[];
extern int data_ov007_02103464;
extern int data_ov007_02103468;
extern int data_ov007_02102dd8;

extern void MultiStore32Bytes(unsigned val, int *dst, int len);
extern void func_ov007_020b8a78(void *t, int idx);
extern void func_ov007_020b8d48(void *t, int v);
extern void func_ov007_020b8b00(void *obj, int arg);
extern int func_ov007_020b8ec0(int x);
extern int func_ov007_020c94a0(int a, unsigned int b);
extern void func_ov007_020b8b80(void *a, int b, int c);


void func_ov007_020b8188(int sl, char *p1, int p2)
{
    volatile int zero;
    void *t = (void *)2;
    int one = 1;
    int f0 = 0xf0;
    int ef = 0xef;
    int c0 = 0xc0;
    int sb;
    int b8, bc, b10;
    int s14;
    int base15;
    int s1c = 0x5d, s20 = 0x4d, s24 = 0x30, s28 = 0, s2c = 0, s30 = 0;
    int s34 = 0xc, s38 = 0, s3c = 0x9c, s40 = 6, s44 = 0, s48 = 7, s4c = 0xee;

    zero = 0;
    MultiStore32Bytes(zero, (int *)data_ov007_02103478[2], data_ov007_020d7a5c[2]);
    base15 = sl * 0xf;
    data_ov007_02102dd8 = 0xe;

    sb = 0;
    s14 = 0;
    for (; sb < 0x12; sb++) {
        data_ov007_02103468 = s14;
        switch (sb) {
        case 0:
            data_ov007_02103464 = s1c;
            func_ov007_020b8a78(t, sl);
            func_ov007_020b8d48(t, s20);
            func_ov007_020b8d48(t, s20);
            func_ov007_020b8d48(t, f0);
            func_ov007_020b8d48(t, ef);
            func_ov007_020b8b00(t, ((s16 *)(p1 + 4))[sl]);
            break;
        case 1:
            data_ov007_02103464 = s24;
            func_ov007_020b8b80(t, func_ov007_020c94a0(s28, (unsigned int)func_ov007_020b8ec0(0x1f57)), one);
            if (p2 != 0) {
                data_ov007_02103464 = c0;
                func_ov007_020b8b80(t, func_ov007_020c94a0(s2c, (unsigned int)func_ov007_020b8ec0(0x1f62)), one);
            } else {
                data_ov007_02103464 = c0;
                func_ov007_020b8b80(t, func_ov007_020c94a0(s30, (unsigned int)func_ov007_020b8ec0(0x1f58)), one);
            }
            break;
        case 0x11:
            data_ov007_02103464 = s34;
            func_ov007_020b8b80(t, func_ov007_020c94a0(s38, (unsigned int)func_ov007_020b8ec0(0x13b0)), one);
            data_ov007_02103464 = s3c;
            func_ov007_020b8d48(t, f0);
            func_ov007_020b8d48(t, ef);
            func_ov007_020b8b00(t, *(u8 *)(sl + *(int *)p1 + 0x98));
            break;
        default: {
            int row = func_ov007_020b8ec0(0x1388);
            row += sb - 2;
            char *m = (char *)(base15 + *(int *)p1);
            m += sb - 2;
            b8 = *(u8 *)(m + 0x11);
            bc = *(u8 *)(m + 0x3e);
            b10 = *(u8 *)(m + 0x6b);
            data_ov007_02103464 = s40;
            func_ov007_020b8b80(t, func_ov007_020c94a0(s44, (unsigned int)row), one);
            data_ov007_02103464 = s3c;
            func_ov007_020b8d48(t, f0);
            func_ov007_020b8d48(t, ef);
            b8 = (b8 >= 8) ? s48 : b8;
            func_ov007_020b8b00(t, b8);
            data_ov007_02103464 = c0;
            func_ov007_020b8d48(t, s4c);
            func_ov007_020b8d48(t, ef);
            func_ov007_020b8b00(t, bc);
            if (b10 != 0)
                func_ov007_020b8d48(t, f0);
            break;
        }
        }
        s14 += 2;
    }
}
