int func_ov071_02120a20(char *c)
{
    if (*(unsigned char *)(c + 0x214) == 0) {
        (*(volatile unsigned char *)(((int)c + 0x214)))++;
        return 1;
    }

    return 0;
}
