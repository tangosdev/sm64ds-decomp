typedef unsigned char u8;

extern u8 BOTTOM_SCREEN_RELATED;
extern int data_ov075_0211d71c[];

extern void func_ov075_02116028(int *p, int v);
extern void func_02034414(unsigned short n);
extern void func_ov075_02115e8c(void *a, int b, int c, int d, int e);
extern void func_02030a38(void);
extern int func_0203da9c(void);
extern int func_ov075_02115098(char *c, int arg1);

void func_ov075_02119918(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;

    BOTTOM_SCREEN_RELATED = BOTTOM_SCREEN_RELATED & ~1;
    func_ov075_02116028(data_ov075_0211d71c, 2);
    func_02034414(0x11);

    BOTTOM_SCREEN_RELATED = BOTTOM_SCREEN_RELATED | 5;
    *(u8 *)(c + 0x281) = 1;
    func_ov075_02115e8c(c + 0x70, 8, 0, 0x50, 0xa8);
    func_ov075_02115e8c(c + 0x94, 8, 0, 0xb0, 0xa8);

    *(u8 *)(c + 0x280) = 2;
    func_02030a38();
    {
        int r = func_0203da9c();
        *(u8 *)(c + 0x282) = (u8)(func_ov075_02115098(*(char **)(c + 0x50), r) != 0);
    }
}
