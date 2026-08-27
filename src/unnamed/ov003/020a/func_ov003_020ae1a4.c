extern int __aeabi_idiv(int a, int b);
extern unsigned short data_ov003_020b16ac[];

void func_ov003_020ae1a4(char *c, int sb)
{
    int found = 0;
    int one = 1;
    int m1 = -1;
    int i;
    for (i = 0; i < 3; i++) {
        unsigned short d = data_ov003_020b16ac[i];
        int q = sb / d;
        if (q == 0 && found == 0 && i != 2) {
            (c + i)[0x121] = (char)m1;
        } else {
            found = one;
            (c + i)[0x121] = (char)q;
        }
        sb = (unsigned short)(sb % d);
    }
}
