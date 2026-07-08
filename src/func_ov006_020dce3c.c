void func_ov006_020dce3c(char* c)
{
    if (*(unsigned char*)(c + 0x4000 + 0xd13) == 0)
        return;
    if (*(unsigned short*)(c + 0x4d08) == *(unsigned short*)(c + 0x4d0a))
        return;
    {
        unsigned short* e = (unsigned short*)(((long long)(int)(c + 0x4d0c)) & 0xFFFFFFFFFFFFFFFFLL);
        *e = *e + 1;
    }
    if (*(unsigned short*)(c + 0x4d0c) < 8)
        return;
    *(unsigned short*)(c + 0x4d0c) = 0;
    {
        unsigned short* p = (unsigned short*)(((long long)(int)(c + 0x4d0a)) & 0xFFFFFFFFFFFFFFFFLL);
        *p = *p + 1;
    }
}
