typedef unsigned char u8;
typedef unsigned short u16;

extern char* data_020a9db4;

#define B (data_020a9db4 + 0x1300)

int func_02068844(void)
{
    int a = *(u16*)(B + 0x10) ? 2 : 0;
    int b = *(u16*)(B + 0xc) ? 1 : 0;
    int c = *(u16*)(B + 0x12) ? 4 : 0;
    int d = *(u16*)(B + 0x14) ? 8 : 0;
    return (d | (c | (b | a))) & 0xff;
}
