extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

void func_ov006_020e2dbc(char *c)
{
    if (*(unsigned char*)(c + 0x4ee7) == 0) return;
    if (*(unsigned short*)(c + 0x4ee0) != 0)
    {
        *(unsigned short *)(((int)c + 0x4ee0) & 0xFFFFFFFFFFFFFFFF) -= 1;
        if ((short)*(unsigned short*)(c + 0x4ee0) <= 0)
            *(unsigned short*)(c + 0x4ee0) = 0;
        return;
    }
    *(unsigned char*)(c + 0x4ee7) = 0;
    {
        int idx = *(unsigned char*)(c + 0x4ee6);
        if (idx >= 5)
            return;
        {
            char *b = c + idx * 0x2c;
            *(unsigned char*)(b + 0x4689) = 1;
            *(unsigned char*)(b + 0x468a) = 1;
            *(int*)(b + 0x4660) = 0x80000;
            *(int*)(b + 0x4664) = 0x80000;
            *(unsigned short*)(b + 0x4680) = 0;
            *(unsigned char*)(b + 0x468b) = 0;
        }
    }
    if (*(unsigned char*)(c + 0x4ee6) != 0)
        _ZN5Sound12PlayBank2_2DEj(0x1d7);
    (*(unsigned char *)(((int)c + 0x4ee6) & 0xFFFFFFFFFFFFFFFF))++;
    *(int*)(c + 0x4eb0) = 0x80000;
    *(int*)(c + 0x4eb4) = 0xb0000;
    *(unsigned char*)(c + 0x4ee4) = 0;
    *(unsigned char*)(c + 0x4ee5) = 1;
}
