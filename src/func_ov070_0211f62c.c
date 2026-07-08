extern int func_02015bcc(void *p);
extern signed char data_0209f2f8;
extern void func_ov070_02120020(void *c, void *p);
extern char data_ov070_0212359c;

int func_ov070_0211f62c(char *c)
{
    if (func_02015bcc(c + 0x350) != 0) {
        if (data_0209f2f8 != 0x16)
            *(int *)(((long long)(int)(c + 0x3c4)) & 0xFFFFFFFFFFFFFFFFLL) += 0x12c000;
        *(int *)(c + 0x3d8) = 0;
        *(short *)(c + 0x3cc) = 0x5a;
        func_ov070_02120020(c, &data_ov070_0212359c);
    }
    return 1;
}
