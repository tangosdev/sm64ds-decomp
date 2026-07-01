typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern int func_0200f0bc(void);
extern int LoadFile(int handle);
extern void func_ov075_02116030(void *c, int v);
extern int GetSoundMode(void);
extern int func_02033ae0(int a);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(void *p, u16 a, u16 b);
extern void func_ov075_02115e8c(char *self, int a, int b, s16 c, s16 e);
extern void func_02020304(void);

extern u8 data_0209d454;
extern int data_ov075_0211ca08[];
extern int data_ov075_0211d71c;
extern short data_ov075_0211d72c[];
extern short data_ov075_0211d740[];
extern u8 data_0208ee3c;

#define SH(base,i) (*(short*)((char*)(base) + (i)*4))

void func_ov075_021173a8(char *c) {
    func_ov075_02116030(&data_ov075_0211d71c, LoadFile(data_ov075_0211ca08[func_0200f0bc()]));

    func_02033ae0((s16)(GetSoundMode() + 9));

    data_0209d454 |= 5;
    _ZN3G2x18SetBlendBrightnessEPVtts((void *)0x4000050, 0xb, (double)(~5));

    *(u8 *)(c + 0x281) = 0;

    func_ov075_02115e8c(c + 0x148, 5, 0, (s16)(SH(data_ov075_0211d72c, func_0200f0bc()) + 8), 0x38);
    func_ov075_02115e8c(c + 0x16c, 6, 0, (s16)(SH(data_ov075_0211d740, func_0200f0bc()) + 8), 0x68);
    func_ov075_02115e8c(c + 0x190, 0xc, 0, 0x56, 0xa8);
    func_ov075_02115e8c(c + 0xb8, 1, 0, 0xac, 0x38);
    func_ov075_02115e8c(c + 0x70, 3, 0, 0x64, 0x38);
    func_ov075_02115e8c(c + 0x94, 4, 0, 0xf4, 0x38);

    *(u8 *)(c + 0xc8) = 1;

    func_ov075_02115e8c(c + 0xdc, 2, 0, 0x8c, 0x68);
    func_ov075_02115e8c(c + 0x100, 2, 0, 0xcc, 0x68);

    if (data_0208ee3c != 0)
        *(u8 *)(c + 0xec) = 1;
    else
        *(u8 *)(c + 0x110) = 1;

    func_ov075_02115e8c(c + 0x124, 0xb, 0, 0x80, 0xa8);

    *(u8 *)(c + 0x280) = 9;

    func_02020304();
}
