typedef unsigned short u16;
extern unsigned func_02062734(char* base, unsigned mask, unsigned acc, int count);

unsigned func_02062778(char* a, unsigned short* b, int c)
{
    if (a == 0) return 0;
    if (b == 0) return 0;
    if ((b[1] & (1u << c)) == 0) return 0;
    return func_02062734(a, b[0], (unsigned)(b + 2), c);
}
