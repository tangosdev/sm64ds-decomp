extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

void func_ov006_0210d8bc(char *c)
{
    (*(int *)(((int)c + 0x40) & 0xFFFFFFFFFFFFFFFF)) += 1;
    if (*(int *)(c + 0x40) == 1) {
        *(int *)(c + 0x34) = 0x14;
        *(unsigned char *)(c + 0x3c) = 1;
        *(int *)(c + 0x38) = 0;
        _ZN5Sound12PlayBank2_2DEj(0x1a1);
        return;
    }
    *(int *)(c + 0x34) = 0;
    *(unsigned char *)(c + 0x3c) = 1;
    *(int *)(c + 0x38) = 0;
    _ZN5Sound12PlayBank2_2DEj(0x1a2);
}
