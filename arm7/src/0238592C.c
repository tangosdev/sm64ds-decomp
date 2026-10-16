/* storage 0x0238592C runtime 0x037FD7C4 size 0x3C wram ARM. No pool. */

extern int fn_02385968(int a1, int a2);

int fn_0238592C(unsigned char *a1) {
    int i;

    for (i = 0; i < 16; i++)
        fn_02385968((int)a1, i);
    *a1 = 0;
    return 0;
}
