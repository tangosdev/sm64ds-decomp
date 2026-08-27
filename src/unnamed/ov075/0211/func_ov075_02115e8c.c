extern int func_0200f0bc(void);
extern void func_ov001_020ab5b0(char* r0, int r1, short r2, short r3, short s4, short s5);
extern short data_ov075_0211d72c[];
extern short data_ov075_0211d72e[];
extern short data_ov075_0211d740[];
extern short data_ov075_0211d742[];
extern short data_ov075_0211d754[];
extern short data_ov075_0211d756[];
extern short data_ov075_0211d948[];
extern short data_ov075_0211d94a[];

#define SH(base,i) (*(short*)((char*)(base) + (i)*4))

void func_ov075_02115e8c(char* self, int a, int b, short c, short e)
{
    int kind;
    int i, j;
    *(int*)(self + 0x1c) = a;
    *(int*)(self + 0x20) = b;
    kind = (*(int*)(self + 0x1c) < 7) ? 2 : (*(int*)(self + 0x1c) < 0xd) ? 1 : 0;
    if (a == 5) {
        i = func_0200f0bc();
        j = func_0200f0bc();
        func_ov001_020ab5b0(self, kind, c, e,
                            SH(data_ov075_0211d72c, i), SH(data_ov075_0211d72e, j));
    } else if (a == 6) {
        i = func_0200f0bc();
        j = func_0200f0bc();
        func_ov001_020ab5b0(self, kind, c, e,
                            SH(data_ov075_0211d740, i), SH(data_ov075_0211d742, j));
    } else if (a == 0xb) {
        i = func_0200f0bc();
        j = func_0200f0bc();
        func_ov001_020ab5b0(self, kind, c, e,
                            SH(data_ov075_0211d754, i), SH(data_ov075_0211d756, j));
    } else {
        func_ov001_020ab5b0(self, kind, c, e,
                            SH(data_ov075_0211d948, a), SH(data_ov075_0211d94a, a));
    }
}
