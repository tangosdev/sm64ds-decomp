typedef unsigned short u16;

extern void func_ov075_02116028(int *p, int v);
extern int LoadFile(int handle);
extern void func_ov075_02116030(void *c, int v);
extern void *_ZN3G2S12GetBG3ScrPtrEv(void);
extern unsigned int LoadCompressedFileAt(u16 fileID, void *target);
extern void _ZN7Message17DisplayVsExitTextEt(u16 a);
extern void func_ov075_02115e8c(void *p, int b, int c, int d, int e);
extern void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void *p, u16 a, u16 b, u16 c, u16 d);
extern void func_02020190(void);
extern void func_02012790(int a);
extern void _ZN5Sound22LoadAndSetMusic_Layer1Ei(int a);

extern unsigned char BOTTOM_SCREEN_RELATED;
extern int data_ov075_0211d71c;
extern unsigned char TOP_SCREEN_RELATED;
extern int DP_STATE;

void func_ov075_02118378(void *c) {
    BOTTOM_SCREEN_RELATED &= ~5;
    func_ov075_02116028(&data_ov075_0211d71c, 1);
    func_ov075_02116030(&data_ov075_0211d71c, LoadFile(0x9801));
    LoadCompressedFileAt(0x8407, _ZN3G2S12GetBG3ScrPtrEv());
    _ZN7Message17DisplayVsExitTextEt(0x1f);

    TOP_SCREEN_RELATED |= 0xe;
    BOTTOM_SCREEN_RELATED |= 5;
    *(unsigned char *)((char *)c + 0x281) = 0;
    func_ov075_02115e8c((char *)c + 0x70, 0xa, 0, 0x80, 0x48);
    func_ov075_02115e8c((char *)c + 0x94, 0xa, 0, 0x80, 0x78);
    *(unsigned char *)((char *)c + 0x280) = 2;
    _ZN3G2x13SetBlendAlphaEPVttttt((volatile void *)0x4000050, 4, 0x11, 0xc, 4);
    *(unsigned char *)((char *)c + 0x287) = 0xe;
    if (DP_STATE != 0)
        DP_STATE = 5;
    func_02020190();
    *(int *)((char *)c + 0x268) = 0;
    *(int *)((char *)c + 0x26c) = 0;
    func_02012790(0x125);
    _ZN5Sound22LoadAndSetMusic_Layer1Ei(0x4c);
}
