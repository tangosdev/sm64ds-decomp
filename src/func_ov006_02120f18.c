struct Obj {
    unsigned char pad0[0x68];
    unsigned char f68;
    unsigned char pad1[3];
    int f6c;
};

extern char *_ZN2G212GetBG0ScrPtrEv(void);
extern char *_ZN2G212GetBG1ScrPtrEv(void);
extern char *_ZN2G212GetBG2ScrPtrEv(void);
extern char *_ZN2G213GetBG2CharPtrEv(void);
extern char *func_02054efc(void);
extern char *func_02054ea8(void);
extern char *func_02054fb0(void);
extern char *func_02054d88(void);
extern void MultiStore16(unsigned short val, char *dst, int nbytes);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *src, unsigned int a, unsigned int b);
extern const unsigned char data_ov006_0212f0d0[];

void func_ov006_02120f18(struct Obj *self, int arg1)
{
    volatile unsigned short t0;
    volatile unsigned short t1;
    int v = 0x7000;
    char *scrPtr;
    char *charPtr;
    int i;

    self->f68 = 1;
    self->f6c = arg1;

    switch (self->f6c) {
    case 0:
        scrPtr = _ZN2G212GetBG0ScrPtrEv();
        charPtr = func_02054efc();
        break;
    case 1:
        scrPtr = _ZN2G212GetBG1ScrPtrEv();
        charPtr = func_02054ea8();
        break;
    case 2:
        scrPtr = _ZN2G212GetBG2ScrPtrEv();
        charPtr = _ZN2G213GetBG2CharPtrEv();
        break;
    case 3:
        scrPtr = func_02054fb0();
        charPtr = func_02054d88();
        break;
    default:
        return;
    }

    for (i = 0; i < 0x300; i++) {
        t0 = v;
        MultiStore16(t0, scrPtr, 2);
        v = (unsigned short)(v + 1);
        scrPtr += 2;
    }

    t1 = 0;
    MultiStore16(t1, charPtr, 0x6000);
    _ZN2GX10LoadBGPlttEPKvjj(data_ov006_0212f0d0, 0xe0, 0x20);
}
