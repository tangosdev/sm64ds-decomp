typedef unsigned char u8;
typedef unsigned short u16;
extern u8 data_02092660;
extern u16 data_02092664;
extern int data_0209f5b8;
extern int func_02013edc(int a, int b, int c);

int _ZN5Scene16SpawnIfNecessaryEv(void)
{
    u16 h;
    if (data_02092660 != 0 || (h = data_02092664) == 0x187)
        return 0;
    {
        int r = func_02013edc(h, data_0209f5b8, 1);
        if (r == 0) return 0;
        data_02092664 = 0x187;
        data_02092660 = 1;
        return r;
    }
}
