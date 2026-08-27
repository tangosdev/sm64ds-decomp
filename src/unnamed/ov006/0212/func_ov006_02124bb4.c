extern void func_ov004_020adb1c(int self);
extern void func_ov004_020b0a54(int v);
extern void func_ov006_020c0c80(void *c);
extern void func_ov006_020c0d68(char *c);

void func_ov006_02124bb4(char *c)
{
    int *cnt;
    int *p;

    cnt = (int *)(((int)c + 0x51bc));
    *cnt = *cnt - 1;
    if (*(int *)(c + 0x5000 + 0x1bc) > 0)
        return;

    if (*(unsigned char *)(c + *(int *)(c + 0x5000 + 0x1c0) + 0x5000 + 0x1ca) >
        *(unsigned char *)(c + *(int *)(c + 0x5000 + 0x1c4) + 0x5000 + 0x1ca))
    {
        if (*(int *)(c + 0xb4) < 0x270f)
        {
            p = (int *)(((int)c + 0xb4));
            *p = *p + 1;
        }
        if (*(int *)(c + 0xb4) > *(int *)(c + 0xb8))
            *(int *)(c + 0xb8) = *(int *)(c + 0xb4);
        func_ov004_020adb1c(*(int *)(c + 0xb4));
        func_ov004_020b0a54(4);
        func_ov006_020c0c80(c + 0x4f38);
        *(int *)(c + 0x5000 + 0x1b8) = 0xd;
    }
    else
    {
        if (*(int *)(c + 0xb4) > 0)
        {
            p = (int *)(((int)c + 0xb4));
            *p = *p - 1;
        }
        func_ov004_020b0a54(5);
        func_ov006_020c0d68(c + 0x4f38);
        *(int *)(c + 0x5000 + 0x1b8) = 0xd;
    }
}
