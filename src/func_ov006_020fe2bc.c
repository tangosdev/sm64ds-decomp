void func_ov006_020fe2bc(char *c)
{
    if (*(unsigned char *)(c + 0x5000 + 0xc32) == 0)
        *(unsigned char *)(c + 0x5c32) += 1;
}
