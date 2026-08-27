int func_ov073_021222c8(void *c)
{
    *(unsigned short*)((char*)c + 0x330) = 0x190;
    int neg = -0x14000;
    *(int*)((char*)c + 0x9c) = neg;
    *(int*)((char*)c + 0xa0) = neg;
    return 1;
}
