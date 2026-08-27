void func_ov098_02138484(char *c)
{
    *(int *)(c + 0x5f0) = 0;
    *(int *)(c + 0x5f4) = 0;
    *(int *)(((int)c + 0xb0)) &= ~0x80000;
    *(unsigned char *)(c + 0x604) = 3;
    *(int *)(c + 0xa8) = *(unsigned char *)(c + 0x604) * 0xa000;
    *(int *)(c + 0x98) = *(unsigned char *)(c + 0x604) * 0x5000;
    *(int *)(c + 0xd0) = 0;
    *(int *)(c + 0x5e4) = 0;
    *(int *)(((int)c + 0x57c)) &= ~0x2000;
}
