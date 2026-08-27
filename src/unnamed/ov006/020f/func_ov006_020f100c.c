typedef int s32;
extern s32 GetGameLanguage(void);
extern void func_ov004_020af948(void *a, int b, int c, void *m);
extern void *data_ov006_0213ce70[];
extern char data_ov006_02137cd8[];
void func_ov006_020f100c(char *c)
{
    int i;
    void *m0 = 0;
    void *m1 = 0;
    void *m2 = 0;
    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char *)(c + 0x4671) != 0) {
            int x = *(s32 *)(c + 0x4660) >> 0xc;
            int y = *(s32 *)(c + 0x4664) >> 0xc;
            s32 r;
            if (x <= 0x10) x = 0x10;
            if (x >= 0xe0) x = 0xe0;
            if (y <= 8) y = 8;
            if (y >= 0xb8) y = 0xb8;
            r = GetGameLanguage();
            func_ov004_020af948(*(void **)((char *)data_ov006_0213ce70[r] + 0x38), x - 0x10, y, m0);
            func_ov004_020af948(*(void **)(data_ov006_02137cd8 + 0xa4), x, y, m1);
            func_ov004_020af948(*(void **)(data_ov006_02137cd8 + 0xa0), x + 0x10, y, m2);
        }
        c += 0x14;
    }
}
