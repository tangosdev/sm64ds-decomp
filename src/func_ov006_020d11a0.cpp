//cpp
extern "C" {
extern void SetSubBg1Offset(int a, int b);
extern int func_ov004_020ad674(void);
extern unsigned int func_02054e88(void);
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern void *_ZN3G2S12GetBG1ScrPtrEv(void);
extern unsigned char BOTTOM_SCREEN_RELATED;
extern int data_ov006_0213b838[];
}
extern "C" void func_ov006_020d11a0(void);
extern "C" void func_ov006_020d11a0(void) {
    volatile unsigned short *reg = (volatile unsigned short*)0x400100a;
    int id;
    void *t;
    *reg = (unsigned short)((*reg & 0x43) | 4);
    *reg = (unsigned short)(*reg & ~0x40);
    *reg = (unsigned short)(*reg & ~3);
    SetSubBg1Offset(0, 0);
    BOTTOM_SCREEN_RELATED = (unsigned char)(BOTTOM_SCREEN_RELATED & ~2);
    id = func_ov004_020ad674();
    t = (void*)func_02054e88();
    LoadCompressedFileAt(data_ov006_0213b838[id], t);
    LoadCompressedFileAt(0x5b, _ZN3G2S12GetBG1ScrPtrEv());
}
