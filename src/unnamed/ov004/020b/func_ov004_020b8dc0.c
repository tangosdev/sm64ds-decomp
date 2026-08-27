extern int GetGameLanguage(void);
extern void func_ov001_020ab5b0(char* r0, int r1, short r2, short r3, short s4, short s5);
extern short data_ov004_020bfe74[];
extern short data_ov004_020bfe76[];
extern short data_ov004_020bfe88[];
extern short data_ov004_020bfe8a[];
extern short data_ov004_020bfe9c[];
extern short data_ov004_020bfe9e[];

#define SH(base,i) (*(short*)((char*)(base) + (i)*4))

void func_ov004_020b8dc0(char* self, int a, int b, short c, short e)
{
    int kind;
    int i, j;
    *(int*)(self + 0x1c) = a;
    kind = (a < 6) ? 2 : 1;
    *(int*)(self + 0x20) = b;
    if (a == 4) {
        i = GetGameLanguage();
        j = GetGameLanguage();
        func_ov001_020ab5b0(self, kind, c, e,
                            SH(data_ov004_020bfe74, i), SH(data_ov004_020bfe76, j));
    } else if (a == 5) {
        i = GetGameLanguage();
        j = GetGameLanguage();
        func_ov001_020ab5b0(self, kind, c, e,
                            SH(data_ov004_020bfe88, i), SH(data_ov004_020bfe8a, j));
    } else {
        func_ov001_020ab5b0(self, kind, c, e,
                            SH(data_ov004_020bfe9c, a), SH(data_ov004_020bfe9e, a));
    }
}
