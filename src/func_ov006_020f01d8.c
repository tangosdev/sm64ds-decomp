typedef int s32;
extern s32 func_ov004_020ad674(void);
extern void func_ov004_020af948(void *a, int b, int c, int d);
extern void *data_ov006_0213ce70[];
extern char data_ov006_02137cd8[];
void func_ov006_020f01d8(char *c)
{
    if (*(unsigned char*)(c + 0x4000 + 0x7e3) == 0) return;
    int x = *(int*)(c + 0x4000 + 0x7d0) >> 0xc;
    int y = *(int*)(c + 0x4000 + 0x7d4) >> 0xc;
    if (x <= 8) x = 8;
    if (x >= 0xf0) x = 0xf0;
    if (y <= 8) y = 8;
    if (y >= 0xb8) y = 0xb8;
    s32 r = func_ov004_020ad674();
    func_ov004_020af948(*(void**)((char*)data_ov006_0213ce70[r] + 0x34), x - 8, y, 0);
    func_ov004_020af948(*(void**)(data_ov006_02137cd8 + 0x64), x + 8, y, 0);
}
