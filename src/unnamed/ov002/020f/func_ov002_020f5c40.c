void func_ov002_020f5c40(char *p)
{
    if (!*(unsigned char *)(p + 0xdc))
        return;
    *(int *)(p + 0x98) = 0x80000;
    *(int *)(p + 0x9c) = 0x60000;
    *(short *)(p + 0xc8) = 0;
    *(unsigned char *)(p + 0xde) = 1;
    *(int *)(p + 0xc0) = 0xccc;
    *(short *)(p + 0xc4) = 0x155;
}
