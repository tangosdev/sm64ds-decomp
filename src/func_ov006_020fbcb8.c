typedef unsigned int u32;

u32 _ZN3G2S13GetBG2CharPtrEv(void);

#pragma opt_loop_invariants off
void func_ov006_020fbcb8(int a0, int a1, int a2, int a3)
{
    int i, j, x, lo, hi, col, row, yb, s;
    u32 *bp;
    s = a1;
    for (i = 0; i < 2; i++)
    {
        x = a2 - 1 + i;
        lo = x & 7;
        hi = x >> 3;
        col = lo * 4;
        for (j = 0; j < 2; j++)
        {
            row = hi * 32;
            yb = s - 1;
            a1 = yb + j;
            bp = (u32 *)_ZN3G2S13GetBG2CharPtrEv();
            *(u32 *)((char *)(bp + (row + (a1 >> 3)) * 8) + col) |= a3 << ((a1 & 7) * 4);
        }
    }
}
