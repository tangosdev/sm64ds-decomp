//cpp
extern "C" {
void func_ov006_020e73c4(void);
void _ZN3GXS11LoadOBJPlttEPKvjj(void const *p, unsigned int a, unsigned int b);
int func_02053eb0(void);
void _ZN2GX15SetBankForSubBGEt(unsigned short x);
int func_02053ea0(void);
void _ZN2GX16SetBankForSubOBJEt(unsigned short x);
void func_ov004_020af094(void *c);
extern void *data_ov006_02141a48[];
extern unsigned char data_0209e660;
void func_ov006_020e6d24(char *c);
}
void func_ov006_020e6d24(char *c) {
    func_ov006_020e73c4();
    _ZN3GXS11LoadOBJPlttEPKvjj(data_ov006_02141a48[0], 0x100, 0x100);
    data_0209e660 = 0;
    *(int *)(c + 0xa0) = func_02053eb0();
    _ZN2GX15SetBankForSubBGEt(0x80);
    *(int *)(c + 0x4660) = func_02053ea0();
    _ZN2GX16SetBankForSubOBJEt(0x100);
    func_ov004_020af094(c);
}
