/* storage 0x02386BFC runtime 0x037FEA94 size 0x58 wram. No pool. */

extern void fn_023835B0(int, int, int);

void fn_02386BFC(unsigned int mask, int a2, int a3) {
    int i;

    i = 0;
    while (i < 16 && mask != 0) {
        if ((mask & 1) != 0)
            fn_023835B0(i, a2, a3);
        i = i + 1;
        mask >>= 1;
    }
}
