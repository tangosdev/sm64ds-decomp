extern int _ZN4cstd4fdivEii(int a, int b);

extern unsigned short data_ov074_02122dfc[];
extern int data_ov074_0212291c[];

int func_ov074_02120b24(char* c)
{
    int num = *(short*)(c + 0x5f6) << 12;
    int den;
    int r;
    if (num < 0) num = -num;
    den = data_ov074_0212291c[*(unsigned char*)(c + 0x604)] + (data_ov074_02122dfc[3] << 12) + 1;
    r = _ZN4cstd4fdivEii(num, den);
    if (r < 0x1000) return 0x1000;
    if (r > 0x3000) return 0x3000;
    return r;
}
