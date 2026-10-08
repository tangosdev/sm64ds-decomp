extern void _ZN4CP1519InvalidateDataCacheEjj(unsigned int addr, unsigned int size);
extern unsigned int *data_020a7fc0;

unsigned int func_0205b608(void)
{
    _ZN4CP1519InvalidateDataCacheEjj((unsigned int)data_020a7fc0, 0x280);
    return data_020a7fc0[1];
}
