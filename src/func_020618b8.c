typedef unsigned short u16;

extern void *WM_GetSystemWork(void);
extern int func_0206152c(void);
extern void _ZN4CP1519InvalidateDataCacheEjj(unsigned int addr, unsigned int n);

struct Obj { int f0; unsigned char *f4; };

int func_020618b8(void)
{
    struct Obj *o = (struct Obj *)WM_GetSystemWork();
    unsigned char *b;
    if (func_0206152c())
        return 0;
    _ZN4CP1519InvalidateDataCacheEjj((unsigned int)o->f4, 2);
    b = o->f4;
    switch (*(u16 *)b) {
    case 9:
        _ZN4CP1519InvalidateDataCacheEjj((unsigned int)(b + 0x17e), 2);
        b = o->f4;
        if (*(u16 *)(b + 0x17e) == 0)
            return 0;
        /* fall through */
    case 0xa:
    case 0xb:
        _ZN4CP1519InvalidateDataCacheEjj((unsigned int)(b + 0xb8), 2);
        return *(u16 *)(o->f4 + 0xb8);
    }
    return 0;
}
