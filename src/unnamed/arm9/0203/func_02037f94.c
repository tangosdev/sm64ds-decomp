int func_02037f94(char *c)
{
    if (*(unsigned short*)(c + 0x1a) != 0x18 && *(unsigned short*)(c + 0x18) != 0xffff)
        return 1;
    return 0;
}
