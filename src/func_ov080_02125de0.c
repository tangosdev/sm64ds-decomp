typedef unsigned int u32;
extern void func_ov080_02125d64(char* c);
extern char data_ov080_021277a8[];

void func_ov080_02125de0(char* c, int a1, int a2, int a3)
{
    u32 m8;
    int r0;
    unsigned char f;
    *(int*)(c + 0x134) = a1;
    *(int*)(c + 0x138) = a2;
    *(int*)(c + 0x13c) = 0;
    func_ov080_02125d64(c);
    m8 = *(u32*)(c + 8);
    r0 = 0;
    if ((unsigned char)((m8 >> 0xd) & 3) == 1) {
        f = (unsigned char)((m8 >> 8) & 0x1f);
        if (f == 4)
            r0 = 0xc;
        else if (f == 7)
            r0 = 0xd;
    } else {
        switch ((unsigned char)(m8 & 0xf) + 1) {
        case 3: r0 = 0; break;
        case 4: r0 = 1; break;
        case 5: r0 = 2; break;
        case 6: r0 = 3; break;
        case 7: r0 = 4; break;
        case 16: r0 = 5; break;
        }
        if (a3 != 0)
            r0 += 6;
    }
    *(char**)(c + 0x1ac) = data_ov080_021277a8 + r0 * 0x14;
    *(unsigned short*)(c + 0x1b6) = *(int*)(*(char**)(c + 0x1ac) + 0x10);
}
