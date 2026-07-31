//cpp
// @symbol func_ov006_020d11a0
// @emits dScMgAmida_c_Kill
/* recovered: renamed to Class_Method */
/* dScMgAmida_c::Kill - recovered from vtable slot identity */
extern "C" {
extern void SetSubBg1Offset(int a, int b);
extern int GetGameLanguage(void);
extern unsigned int _ZN3G2S13GetBG1CharPtrEv(void);
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern void *_ZN3G2S12GetBG1ScrPtrEv(void);
extern unsigned char data_0209d454;
extern int data_ov006_0213b838[];
}
extern "C" void dScMgAmida_c_Kill(void);
extern "C" void dScMgAmida_c_Kill(void) {
    volatile unsigned short *reg = (volatile unsigned short*)0x400100a;
    int id;
    void *t;
    *reg = (unsigned short)((*reg & 0x43) | 4);
    *reg = (unsigned short)(*reg & ~0x40);
    *reg = (unsigned short)(*reg & ~3);
    SetSubBg1Offset(0, 0);
    data_0209d454 = (unsigned char)(data_0209d454 & ~2);
    id = GetGameLanguage();
    t = (void*)_ZN3G2S13GetBG1CharPtrEv();
    LoadCompressedFileAt(data_ov006_0213b838[id], t);
    LoadCompressedFileAt(0x5b, _ZN3G2S12GetBG1ScrPtrEv());
}
