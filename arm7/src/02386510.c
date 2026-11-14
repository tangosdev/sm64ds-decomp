extern int fn_023864E0(int, unsigned short, unsigned short);
extern int fn_02384628(int, int, int, int);
extern int fn_023845D4(int, unsigned short, int);
extern int fn_02384588(int, int);
extern void fn_02384564(int, int);
extern void fn_02384548(int, int);
extern void fn_02384540(int, int);
extern void fn_02384524(int, int);

int fn_02386510(unsigned char *a1, unsigned char a2, unsigned char a3, int a4, int a5, unsigned char *a6) {
    int v10;
    int v11;
    switch (*a6) {
    case 1:
    case 4:
        if (*a6 == 1)
            v10 = fn_023864E0(a5, *(unsigned short *)(a6 + 4), *(unsigned short *)(a6 + 2));
        else
            v10 = (*(volatile unsigned short *)(a6 + 4) << 16) | *(volatile unsigned short *)(a6 + 2);
        if (v10 != 0)
            v11 = fn_02384628((int)a1, v10, v10 + 0xC, a4);
        else
            v11 = 0;
        break;
    case 2:
        v11 = fn_023845D4((int)a1, *(unsigned short *)(a6 + 2), a4);
        break;
    case 3:
        v11 = fn_02384588((int)a1, a4);
        break;
    default:
        v11 = 0;
        break;
    }
    if (v11 == 0)
        return 0;
    a1[8] = a2;
    a1[5] = a6[6];
    a1[9] = a3;
    fn_02384564((int)a1, a6[7]);
    fn_02384548((int)a1, a6[8]);
    fn_02384540((int)a1, a6[9]);
    fn_02384524((int)a1, a6[10]);
    a1[10] = a6[11] - 0x40;
    return 1;
}
