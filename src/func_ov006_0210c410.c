extern int func_ov001_020ab450(void *c, int x);
extern void func_ov006_0210c2d4(void *c);
extern void func_ov004_020b1b40(int x);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern int func_ov001_020ab550(void *c);
void func_ov006_0210c410(char *c) {
    int val;
    if (func_ov001_020ab450(c, -1)) func_ov006_0210c2d4(c);
    val = *(int *)(c + 0x20);
    if (val != 0) {
        if ((val & 3) == 0) {
            func_ov004_020b1b40(1);
            _ZN5Sound12PlayBank2_2DEj(0x149);
        }
        int *p = (int *)(((int)c + 0x20) & 0xFFFFFFFFFFFFFFFF);
        *p = *p - 1;
    }
    func_ov001_020ab550(c);
}
