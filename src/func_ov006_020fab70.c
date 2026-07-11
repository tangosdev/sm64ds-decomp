extern void SetBg2Offset(int a, int b);

void func_ov006_020fab70(char* c, int i)
{
    int off = i * 0x14;
    int* p04;
    int* p08;
    int* pfc;
    int* p00;
    int a;
    int b;

    *(int*)(c + 0x5bfc + off) += *(int*)(c + 0x5c04 + off);
    *(int*)(c + 0x5c00 + off) += *(int*)(c + 0x5c08 + off);
    p04 = (int*)(c + 0x5c04 + off);
    pfc = (int*)(c + 0x5bfc + off);
    p00 = (int*)(c + 0x5c00 + off);
    p08 = (int*)(c + 0x5c08 + off);
    if (*p04 <= -0x2000) *p04 -= 0x100;
    if (*p08 <= -0xc00) *p08 += 0x40;
    a = *p00 >> 12;
    b = *pfc >> 12;
    if (a <= 0) {
        a = 0;
        *p00 = 0;
        *p08 = 0;
        *(unsigned char*)(c + off + 0x5c0f) = 2;
    }
    SetBg2Offset(b, a);
}
