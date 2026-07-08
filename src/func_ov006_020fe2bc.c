void func_ov006_020fe2bc(char *c)
{
    if (*(unsigned char *)(c + 0x5000 + 0xc32) == 0)
        *(unsigned char *)(((long long)(int)(c + 0x5c32)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
}
