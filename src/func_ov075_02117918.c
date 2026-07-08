typedef short s16;

extern void func_ov075_02115e8c(char *self, int a, int b, s16 c, s16 e);
extern int func_0200f0bc(void);
extern int LoadFile(int handle);
extern void func_ov075_02116030(void *c, int v);
extern void SetBg2Offset(int a, int b);
extern void func_02033bb8(int a);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(void *p, unsigned short a, s16 b);
extern void func_02020304(void);

extern unsigned char data_020a0e40[];
extern unsigned char data_0209f4ae[];
extern s16 data_ov075_0211b5d4[];
extern unsigned char BOTTOM_SCREEN_RELATED[];
extern int data_ov075_0211c9b8[];
extern int data_ov075_0211d71c[];
extern unsigned char TOP_SCREEN_RELATED[];

void func_ov075_02117918(char *self)
{
    int i;
    int r;
    int v;
    char *p;

    i = 0;
    p = self + 0x70;
    v = 0x30;
    *(unsigned char *)(self + 0x281) = data_0209f4ae[data_020a0e40[0] * 0x18];

    for (; i <= 2; i++) {
        func_ov075_02115e8c(p, 0, 0, 0x80, (s16)v);
        v += 0x28;
        p += 0x24;
    }

    *(unsigned char *)(self + *(unsigned char *)(self + 0x281) * 0x24 + 0x80) = 1;

    func_ov075_02115e8c(self + 0xdc, 0xb, 0, 0x80, 0xa8);

    r = func_0200f0bc();
    func_ov075_02115e8c(self + 0x100, 0xc, 0, data_ov075_0211b5d4[r], 0xa8);

    *(unsigned char *)(self + 0x280) = 5;
    BOTTOM_SCREEN_RELATED[0] &= ~5;

    r = func_0200f0bc();
    func_ov075_02116030(data_ov075_0211d71c, LoadFile(data_ov075_0211c9b8[r]));

    if (*(unsigned char *)(self + 0x281) == 0)
        SetBg2Offset(0, 0);
    else if (*(unsigned char *)(self + 0x281) == 1)
        SetBg2Offset(0x100, 0);
    else
        SetBg2Offset(0, 0x100);

    func_02033bb8(5);
    TOP_SCREEN_RELATED[0] |= 4;
    BOTTOM_SCREEN_RELATED[0] |= 5;
    _ZN3G2x18SetBlendBrightnessEPVtts((void *)0x4000050, 0xb, ~5);

    *(int *)(self + 0x268) = *(unsigned char *)(self + 0x281);
    func_02020304();
}
