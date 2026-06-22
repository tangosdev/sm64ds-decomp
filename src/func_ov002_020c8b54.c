int func_ov002_020c8b54(char* c)
{
    if (*(unsigned short*)(c + 0x6a4) == 0) {
        *(int*)(c + 0x98) = 0xa000;
        *(unsigned char*)(c + 0x6e3) = 0xf;
    }
    return 0;
}
