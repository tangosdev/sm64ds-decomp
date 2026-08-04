void func_ov006_020d6170(char* c, int idx)
{
    char* base = c + (idx << 4);
    unsigned short* t = (unsigned short*)(base + 0x6288);

    if (*t != 0) {
        unsigned short* p = (unsigned short*)(c + 0x6288 + (idx << 4));
        *p = (unsigned short)(*p - 1);
        if (*(short*)(base + 0x6288) < 0)
            *(short*)(base + 0x6288) = 0;
        return;
    }

    *(unsigned char*)(base + 0x628c) = 3;
}
