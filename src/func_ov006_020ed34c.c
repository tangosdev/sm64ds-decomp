extern void func_ov004_020b0a54(int c);
extern void func_ov006_020ed32c(char *p);
extern void func_ov006_020ed300(char *p);
extern void func_ov006_020eb8f0(int c);
extern void func_ov006_020eb7f8(int c);

void func_ov006_020ed34c(char *p)
{
    int v;
    *(int *)(((int)p + 0x466c) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
    v = *(int *)(p + 0x466c);
    if (v == 0)
    {
        if (*(unsigned char *)(p + 0x4f64) != 0)
        {
            func_ov004_020b0a54(1);
            func_ov006_020ed32c(p);
        }
        else
        {
            *(int *)(*(int *)(p + 0x4f60) + 0x70) = 1;
            func_ov004_020b0a54(0x12);
            func_ov006_020ed300(p);
        }
        *(unsigned char *)(p + 0xc3) = 0;
    }
    else if (*(unsigned char *)(p + 0x4f64) != 0)
    {
        if (v == 0x5a)
        {
            func_ov006_020eb8f0(*(int *)(p + 0x4f60));
        }
    }
    else
    {
        if (v == 0x8a)
        {
            func_ov006_020eb7f8(*(int *)(p + 0x4f60));
        }
    }
}
