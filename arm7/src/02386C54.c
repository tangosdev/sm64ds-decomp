/* storage 0x02386C54 runtime 0x037FEAEC size 0x48 wram. No pool. */

extern void fn_02383598(int, int);

void fn_02386C54(unsigned int mask, int arg) {
    int i;

    i = 0;
    while (i < 16 && mask != 0) {
        if ((mask & 1) != 0)
            fn_02383598(i, arg);
        i = i + 1;
        mask >>= 1;
    }
}
