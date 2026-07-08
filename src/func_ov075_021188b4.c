typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

extern int func_0203da9c(void);
extern void* _ZN3G2S12GetBG3ScrPtrEv(void);
extern void* _ZN3G2S12GetBG0ScrPtrEv(void);
extern unsigned LoadCompressedFileAt(u16 fileID, void *target);
extern void func_02020820(void *dst, void *src, int a, int b, int c, int d);
extern void MultiStore_Int(int val, void *dst, int len);
extern void func_02033390(int a);
extern void func_020331fc(int a, int b, int c);
extern void func_02020304(void);

extern u8 data_0209b2f0[];
extern int data_ov075_0211ca64;
extern int data_ov075_0211ca6c;
extern int data_ov075_0211ca74;
extern int data_ov075_0211cc44;
extern int data_ov075_0211cc4c;
extern int data_ov075_0211cc54;
extern int data_ov075_0211cc5c;
extern u8 BOTTOM_SCREEN_RELATED;

void func_ov075_021188b4(char *c)
{
    int id;

    id = func_0203da9c();
    *(int *)(c + 0x264) = 0x258;
    *(int *)(c + 0x268) = 0;
    *(int *)(c + 0x26c) = 0;

    if (data_0209b2f0[id] != 0) {
        LoadCompressedFileAt(0x8404, _ZN3G2S12GetBG3ScrPtrEv());
        func_02020820(c + 0x1b4, &data_ov075_0211ca64, 1, 1, 1, 1);
        func_02020820(c + 0x1e0, &data_ov075_0211ca6c, 1, 0, 0, 0);
        func_02020820(c + 0x20c, &data_ov075_0211ca74, 1, 1, 1, 1);
    } else {
        LoadCompressedFileAt(0x8401, _ZN3G2S12GetBG3ScrPtrEv());
        func_02020820(c + 0x1b4, &data_ov075_0211cc44, 1, 1, 1, 1);
        func_02020820(c + 0x1e0, &data_ov075_0211cc4c, 1, 0, 0, 0);
        func_02020820(c + 0x20c, &data_ov075_0211cc54, 1, 0, 0, 0);
        func_02020820(c + 0x238, &data_ov075_0211cc5c, 1, 0, 0, 0);
    }

    c[0x1d6] = id + 0xa;
    c[0x202] = id + 0xa;

    {
        void *bg0 = _ZN3G2S12GetBG0ScrPtrEv();
        volatile int zero = 0;
        MultiStore_Int(zero, bg0, 0x800);
    }

    func_02033390(0);

    func_020331fc(0x40, 0x20, func_0203da9c() << 24 >> 24);

    BOTTOM_SCREEN_RELATED |= 1;

    func_02020304();

    *(int *)(c + 0x274) = 0;
}
