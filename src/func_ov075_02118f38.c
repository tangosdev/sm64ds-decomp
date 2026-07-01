typedef unsigned short u16;

extern void func_ov075_02116028(int *p, int v);
extern int LoadFile(int handle);
extern void func_ov075_02116030(void *c, int v);
extern void *_ZN3G2S12GetBG3ScrPtrEv(void);
extern unsigned int LoadCompressedFileAt(u16 fileID, void *target);
extern int func_0203d9b4(void);
extern void func_020338b0(int a, int b, int c, int d);
extern void func_ov075_02115e8c(char *self, int a, int b, short c, short e);
extern void func_02020334(void);
extern void func_020200e0(void);

extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
extern int data_ov075_0211d71c;
extern unsigned char data_0209b2e4;

void func_ov075_02118f38(void *c) {
    int i;
    int sb;
    char *p;
    char *q;

    data_0209d45c &= ~4;
    data_0209d454 &= ~5;
    func_ov075_02116028(&data_ov075_0211d71c, 1);
    func_ov075_02116030(&data_ov075_0211d71c, LoadFile(0x8409));
    LoadCompressedFileAt(0x8407, _ZN3G2S12GetBG3ScrPtrEv());

    if (func_0203d9b4() != 0)
        func_020338b0(0x205, 0xb0, 0x21, -1);
    else
        func_020338b0(0x225, 0xb0, 0x1d, -1);

    data_0209d454 |= 5;

    p = (char *)c + 0x70;
    q = (char *)c;
    i = 0;
    sb = 0x20;
    for (; i < 4; i++) {
        func_ov075_02115e8c(p, 7, i, (short)sb, 0x48);
        *(unsigned char *)(q + 0x80) = 1;
        *(int *)(q + 0x88) = 8;
        sb += 0x40;
        p += 0x24;
        q += 0x24;
    }

    data_0209b2e4 = 4;
    func_ov075_02115e8c((char *)c + 0x100, 0xd, 4, 0x80, -0x14);
    *(unsigned char *)((char *)c + 0x280) = 5;
    func_02020334();
    func_020200e0();
}
