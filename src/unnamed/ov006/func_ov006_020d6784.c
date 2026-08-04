extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

#pragma opt_strength_reduction off
void func_ov006_020d6784(char *c)
{
    int b, a, i;
    a = 0;
    b = 0;
    for (i = 0; i < 0x70; i++) {
        if (*(unsigned char *)(c + (i << 6) + 0x4698) == 0) continue;
        if (*(unsigned char *)(c + (i << 6) + 0x4697) != 5) continue;
        if (*(unsigned char *)(c + (i << 6) + 0x4696) != 0) a++;
        else b++;
    }
    if (b >= 0x28) {
        *(int *)(c + 0x62d0) = 3;
        *(int *)(c + 0x62d4) = 0;
        *(unsigned char *)(c + 0x62f8) = 0;
        *(unsigned char *)(c + 0x62f5) = 0;
        *(unsigned int *)(c + 0xbc) += 1;
        if (*(unsigned int *)(c + 0xbc) > 0x270e) *(unsigned int *)(c + 0xbc) = 0x270e;
        *(unsigned char *)(c + 0x62fb) = 1;
        _ZN5Sound12PlayBank2_2DEj(0x1e6);
    } else if (a >= 0x28) {
        *(int *)(c + 0x62d0) = 3;
        *(int *)(c + 0x62d4) = 0;
        *(unsigned char *)(c + 0x62f8) = 0;
        *(unsigned char *)(c + 0x62f5) = 1;
        *(unsigned int *)(c + 0xbc) += 1;
        if (*(unsigned int *)(c + 0xbc) > 0x270e) *(unsigned int *)(c + 0xbc) = 0x270e;
        *(unsigned char *)(c + 0x62fb) = 1;
        _ZN5Sound12PlayBank2_2DEj(0x1e6);
    }
}
