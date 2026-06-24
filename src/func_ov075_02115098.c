extern int func_ov075_02114a58(int *p);
extern int func_0203da9c(void);
extern int func_ov075_02114988(char *c);
extern void func_ov075_021151b4(char *c, int idx);

int func_ov075_02115098(char *c, int arg1)
{
    if (*(unsigned char *)(c + 0xf40) != 0)
        return 1;
    int i;
    char *m = c + 0x920;
    for (i = 0; i < 4; i++) {
        if (func_ov075_02114a58((int *)m) == 0)
            return 0;
        m += 0x158;
    }
    int x = func_0203da9c();
    if (arg1 == x) {
        func_ov075_02114988(c + 0x920 + arg1 * 0x158);
        func_ov075_021151b4(c, arg1);
        *(unsigned char *)(c + 0xf40) = 1;
    }
    return 1;
}
