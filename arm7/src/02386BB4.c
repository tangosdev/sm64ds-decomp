/* storage 0x02386BB4 runtime 0x037FEA4C size 0x48 wram. No pool. */

extern void fn_0238353C(int, int);

void fn_02386BB4(unsigned int mask, int arg) {
    int i;

    i = 0;
    while (i < 16 && mask != 0) {
        if ((mask & 1) != 0)
            fn_0238353C(i, arg);
        i = i + 1;
        mask >>= 1;
    }
}
