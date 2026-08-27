extern void func_02012790(int x);
extern int _Z14ApproachLinearRiii(int* v, int target, int step);
extern unsigned short data_0209b274;

int func_ov002_020f7384(char* c, unsigned char* flag, int val)
{
    if (*flag != 0) {
        if (val == data_0209b274) func_02012790(0x17);
        _Z14ApproachLinearRiii((int*)(c + 0x60), 0xfa000, 0xa000);
        c[0x102] = 0x1f;
    } else {
        if (val == data_0209b274) func_02012790(0x16);
        if (_Z14ApproachLinearRiii((int*)(c + 0x60), 0x32000, 0x5000) != 0)
            c[0x102] = 0;
    }
    return 1;
}
