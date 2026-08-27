extern void _ZN5Sound12PlayBank2_2DEj(unsigned int a);

void func_ov006_02120a18(unsigned short* a, int b)
{
    a[0] = b;
    a[1] = 0x3c;
    *(int*)(a + 4) = 1;
    *(int*)(a + 2) = 1;
    _ZN5Sound12PlayBank2_2DEj(0x1B7);
}
