typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;

extern void func_ov004_020b0a54(int);

void func_ov006_020d8f34(char *c)
{
    if (*(u16 *)(c + 0x62e8) == 0)
        return;
    *(u16 *)(((long long)(int)(c + 0x62e8)) & 0xFFFFFFFFFFFFFFFFLL) -= 1;
    if (*(s16 *)(c + 0x62e8) > 0)
        return;
    *(u16 *)(c + 0x62e8) = 0;
    func_ov004_020b0a54(0x10);
    *(u8 *)(c + 0xc3) = 0;
}
