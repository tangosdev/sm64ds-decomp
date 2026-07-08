int func_ov002_020d6084(char *c)
{
    unsigned int *p = (unsigned int *)(((long long)(int)(c + 0x2ec)) & 0xFFFFFFFFFFFFFFFFLL);
    *p &= ~0x2000;
    *p &= ~2;
    *(unsigned char *)(c + 0x713) = 1;
    *(unsigned char *)(c + 0x6f5) = 0x1f;
    return 1;
}
