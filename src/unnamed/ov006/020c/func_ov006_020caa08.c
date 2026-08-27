extern int data_ov006_021405bc;
extern char *data_ov006_02140554;
extern int data_ov006_021405a4;
extern void func_ov006_020ca374(char *o, short v);
extern void func_ov006_020c8ecc(char *o);

void func_ov006_020caa08(void)
{
    int n;
    int i;
    n = data_ov006_021405bc;
    if (n > 3) n = 3;
    for (i = 0; i < n; i++) {
        func_ov006_020ca374(data_ov006_02140554 + i * 0xdc, (short)(i * 0x14));
        *(int *)(data_ov006_02140554 + i * 0xdc + 0x24) = i * 0x10000 - 0x20000;
        *(int *)(data_ov006_02140554 + i * 0xdc + 0x3c) =
            (int)(((long long)data_ov006_021405a4 * (i * 0x800 - 0x1000) + 0x800) >> 0xc);
        func_ov006_020c8ecc(data_ov006_02140554 + i * 0xdc);
    }
}
