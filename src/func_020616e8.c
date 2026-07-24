typedef unsigned short u16;

extern void func_0205b858(void);
extern int func_0205ba3c(int bit, int word);
extern void _ZN4CP1519InvalidateDataCacheEjj(unsigned int, unsigned int);
extern void DMASyncFillTransfer(int ch, void *dst, int val, int len);
extern void func_0206116c(void);
extern int func_02061c88(int idx, int b, int c);
extern void func_02058940(void *s, int *v1, int v2);
extern int func_02058894(void *q, void *val, int flag);
extern void func_0205ba64(int idx, void *fn);

extern u16 data_020a89a8;
extern int *data_020a89ac;
extern int data_020a89b0;
extern int data_020a89c4;
extern char data_020a8a40[];
extern void func_02061188(void);

int func_020616e8(void *buf, int ch)
{
    if (data_020a89a8 != 0)
        return 3;
    data_020a89a8 = 1;
    if (buf == 0) {
        data_020a89a8 = 0;
        return 6;
    }
    if ((unsigned int)ch > 3) {
        data_020a89a8 = 0;
        return 6;
    }
    if (((int)buf & 0x1f) != 0) {
        data_020a89a8 = 0;
        return 6;
    }

    func_0205b858();
    if (func_0205ba3c(0xa, 1) == 0) {
        data_020a89a8 = 0;
        return 4;
    }

    _ZN4CP1519InvalidateDataCacheEjj((unsigned int)buf, 0x1300);
    DMASyncFillTransfer(ch, buf, 0, 0x1300);

    data_020a89ac = (int *)buf;
    data_020a89ac[0] = (int)buf + 0x600;
    data_020a89ac[1] = data_020a89ac[0] + 0x100;
    data_020a89ac[3] = data_020a89ac[1] + 0xa00;
    data_020a89ac[4] = data_020a89ac[3] + 0x100;

    func_0206116c();
    *(u16 *)((char *)data_020a89ac + 0x560) = (u16)ch;
    data_020a89ac[0x4f] = 0;

    {
        int i;
        for (i = 0; i < 0x10; i++)
            func_02061c88((u16)i, 0, 0);
    }

    func_02058940(&data_020a89b0, &data_020a89c4, 0xa);
    {
        int i;
        char *p = &data_020a8a40[0];
        for (i = 0; i < 0xa; i++) {
            func_02058894(&data_020a89b0, p, 1);
            p += 0x100;
        }
    }

    func_0205ba64(0xa, (void *)func_02061188);
    return 0;
}
