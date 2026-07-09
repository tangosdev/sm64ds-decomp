//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void* c, unsigned int a, int b, int d, unsigned int e);
extern int _ZN6Player9GetHealthEv(void* c);

extern unsigned char data_0209f2d8;
extern int data_0209caa0[];

int _ZN6Player12St_Wait_InitEv(char* c)
{
    unsigned char f2d8;
    int b0;

    *(unsigned char*)(c + 0x6e6) = 0;
    f2d8 = data_0209f2d8;
    b0 = (f2d8 == 1);
    if (b0 != 0) goto L80;
    if ((data_0209caa0[2] & 0x80) == 0) goto L50;
    {
        int b1 = (f2d8 == 2);
        if (b1 == 0) goto L80;
    }
L50:
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xb3, 0, 0x1000, 0);
    *(unsigned char*)(c + 0x6e3) = 0;
    return 1;

L80:
    if (*(unsigned char*)(c + 0x703) == 0) goto Ld8;
    if (b0 != 0) goto Lbc;
    if (data_0209caa0[2] & 0x80) goto Lbc;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xa0, 0, 0x1000, 0);
Lbc:
    *(unsigned short*)(c + 0x6a4) = 0x384;
    return 1;

Ld8:
    if (*(int*)(c + 0x650) == 0x80000000) goto L13c;
    if (*(int*)(c + 0x60) >= *(int*)(c + 0x650) - 0x64000) goto L13c;
    if (*(unsigned char*)(c + 0x6f9) != 0) goto L13c;
    if (*(unsigned char*)(c + 0x6fb) != 0) goto L13c;
    *(unsigned char*)(c + 0x6e3) = 0xa;
    _ZN6Player7SetAnimEji5Fix12IiEj(c, 5, 0, 0x1000, 0);
    return 1;

L13c:
    *(unsigned char*)(c + 0x6e3) = 0;
    if (*(int*)(c + 0x68c) > 0x32000) {
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x7d, 0, 0x1000, 0);
    } else if (_ZN6Player9GetHealthEv(c) <= 2) {
        *(unsigned char*)(c + 0x6e3) = 7;
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xe, 0x40000000, 0x1000, 0);
    } else {
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
    }
    *(unsigned short*)(c + 0x6a4) = 0x384;
    return 1;
}
}
