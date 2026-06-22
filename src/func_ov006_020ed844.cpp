//cpp
extern "C" {
extern int _Z14ApproachLinearRiii(int &v, int a, int b);
extern void func_ov004_020b0aa0(int x);
extern void func_ov006_020ed81c(char *p);

void func_ov006_020ed844(char *c)
{
    if (_Z14ApproachLinearRiii(*(int *)(c + 0x4668), 0, 1) == 0) return;
    func_ov004_020b0aa0(0x1d);
    if (*(unsigned char *)(c + 0xc4) == 0) {
        *(unsigned char *)(c + 0xc3) = 1;
        *(unsigned char *)(c + 0xc4) = 1;
        *(short *)(c + 0xc0) = 0;
    }
    func_ov006_020ed81c(c);
}
}
