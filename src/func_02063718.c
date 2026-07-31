typedef unsigned short u16;
typedef unsigned char u8;

extern void MultiCopy_Int(void *dst, void *src, int n);
extern void func_02065bf4(void);
extern int func_02065c1c(void);
extern int func_02065bc0(void);
extern int func_02065bd0(void);
extern int func_02064f54(char *q, int a, int b, int c, int d);
extern void func_020647fc(char *a, int b, void *c, int d);
extern int func_02064e18(char *q, int a, int b, int c, int d);
extern int func_02064cd8(char *q, int a, int b, int c, int d);
extern int func_02064a94(char *q, int a, int b, char *c, int d);

int func_02063718(char *p, char *q)
{
    int t;
    int ret = 0;
    int *st = (int *)((long long)(unsigned int)(p + 0x9c));
    int b;
    int i;
    char *ptr;
    if (*(int *)p != 0) return ret;
    if (*st == 0) return ret;
    func_02065bf4();
    t = func_02065c1c();
    switch (*st) {
    case 2:
        MultiCopy_Int(st, p, 0x9c);
        *(u8 *)(p + 0xc) = t;
        ret = func_02065bc0();
        b = func_02065bd0();
        ret = func_02064f54(q, *(u16 *)(p + 8), *(u8 *)(p + 0xc), ret, b);
        break;
    case 6:
        MultiCopy_Int(st, p, 0x9c);
        *(u8 *)(p + 0xc) = t;
        ptr = p + 0x1f8;
        for (i = 0; i < 0x10; i++, ptr += 0x68) {
            if ((*(u16 *)(p + 8) >> i) & 1)
                func_020647fc(ptr, *(int *)(p + 0x18), ptr + 0x1c, ((int *)p + i)[7]);
        }
        if (*(int *)((long long)(unsigned int)(p + 0x9c)) == 4)
            ret = func_02064e18(q, *(u16 *)(p + 8), *(u8 *)(p + 0xc), *(int *)(p + 0x14), 0);
        else
            ret = func_02064cd8(q, *(u16 *)(p + 8), *(u8 *)(p + 0xc), *(int *)(p + 0x14), 0);
        break;
    case 4:
        MultiCopy_Int(st, p, 0x9c);
        *(u8 *)(p + 0xc) = t;
        ptr = p + 0x1f8;
        for (i = 0; i < 0x10; i++, ptr += 0x68) {
            if ((*(u16 *)(p + 8) >> i) & 1)
                func_020647fc(ptr, *(int *)(p + 0x18), (void *)((int *)p + i)[23], ((int *)p + i)[7]);
        }
        if (*(int *)((long long)(unsigned int)(p + 0x9c)) == 4)
            ret = func_02064e18(q, *(u16 *)(p + 8), *(u8 *)(p + 0xc), *(int *)(p + 0x14), 0);
        else
            ret = func_02064cd8(q, *(u16 *)(p + 8), *(u8 *)(p + 0xc), *(int *)(p + 0x14), 0);
        break;
    case 0xa:
        MultiCopy_Int(st, p, 0x9c);
        *(u8 *)(p + 0xc) = t;
        ret = func_02064a94(q, *(u16 *)(p + 8), *(u8 *)(p + 0xc), p + 0x14, *(u8 *)(p + 0x1d));
        break;
    }
    *(int *)((long long)(unsigned int)(p + 0x9c)) = 0;
    return ret;
}
