extern void *func_02061548(void);
extern int func_020614d0(void);
extern void _ZN4CP1519InvalidateDataCacheEjj(unsigned addr, unsigned len);

unsigned short func_02061960(void)
{
    unsigned char *o = (unsigned char *)func_02061548();
    if (func_020614d0() != 0) return 0;
    {
        unsigned char *p = *(unsigned char **)(o + 4);
        _ZN4CP1519InvalidateDataCacheEjj((unsigned)(p + 0x1f2), 2);
        return *(unsigned short *)(*(unsigned char **)(o + 4) + 0x1f2);
    }
}
