void func_ov006_020e9bbc(char* c)
{
    char* r2 = c + 0x5500;
    if (*(unsigned short*)(r2 + 0x48) != 0) {
        unsigned short* p = (unsigned short*)(((long long)(int)(c + 0x5548)));
        *p = *p - 1;
        if (*(short*)(r2 + 0x48) < 0)
            *(unsigned short*)(r2 + 0x48) = 0;
        return;
    }
    *(int*)(c + 0x5000 + 0x53c) = 1;
    *(unsigned short*)(r2 + 0x48) = 0x40;
}
