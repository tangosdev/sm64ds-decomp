extern unsigned char data_0209d460;
extern int data_ov006_0212e850[];

void func_ov006_020f0bf0(char* c, int i) {
    unsigned short t;
    int x;
    int v;

    t = *(unsigned short*)(c + 0x47b0 + i * 0x18);
    if (t != 0) {
        *(short*)(c + 0x47b0 + i * 0x18) = t - 1;
        if (*(short*)(c + 0x47b0 + i * 0x18) < 0)
            *(short*)(c + 0x47b0 + i * 0x18) = 0;
        return;
    }

    x = *(unsigned short*)0x400004a;
    x &= ~0x3f00;
    x |= 0x1800;
    x &= ~0x2000;
    x |= 0x2000;
    *(unsigned short*)0x400004a = x;
    x = (*(unsigned short*)0x400004a & ~0x3f) | 0x14;
    *(unsigned short*)0x400004a = x;
    *(volatile int*)0x4000000 = (*(volatile int*)0x4000000 & ~0xe000) | 0x8000;
    data_0209d460 = 4;

    *(int*)(c + 0x47a0 + i * 0x18) = data_ov006_0212e850[i] << 12;
    *(int*)(c + 0x47a4 + i * 0x18) = 0x60000;
    *(char*)(c + 0x47b5 + i * 0x18) = 1;
    *(char*)(c + 0x47b6 + i * 0x18) = 1;
    if (i != 0)
        *(int*)(c + 0x47a8 + i * 0x18) = -0x8000;
    else
        *(int*)(c + 0x47a8 + i * 0x18) = 0x8000;

    v = *(int*)(c + 0xbc);
    while (v >= 5)
        v -= 5;
    if (v != 0) {
        *(char*)(c + 0x47b6 + i * 0x18) = 2;
        if (i != 0)
            *(int*)(c + 0x47a8 + i * 0x18) = -0x7000;
        else
            *(int*)(c + 0x47a8 + i * 0x18) = 0x7000;
    }
    *(int*)(c + 0x47ac + i * 0x18) = 0;
}
