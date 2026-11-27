int fn_02387C40(unsigned short *a1)
{
    unsigned short v1 = *(unsigned short *)0x4000006;
    unsigned short v2 = *a1;
    int v3 = v1 - v2;
    if (v3 != 0) {
        *a1 = v1;
        *(int *)(a1 + 2) -= v3;
        if (v3 < 0)
            *(int *)(a1 + 2) -= 0x107;
    }
    return *(int *)(a1 + 2) > 0;
}
