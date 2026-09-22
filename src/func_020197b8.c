extern int data_0209d50c;
extern unsigned char data_0209d4e8;
extern unsigned char data_020a0e40;
extern unsigned short data_020a0e58[];
extern unsigned short data_0209d534[];
extern int data_0209d51c;
extern int data_020a0db0;
extern unsigned char data_0209d4f0;
extern unsigned char data_0209ee90[];
extern int data_0209d514;
extern unsigned char data_0209b480;
extern unsigned char data_0209e64c;

void _ZN3G2x18SetBlendBrightnessEPVtts(void* p, unsigned short a, unsigned short b);
void func_02019ac4(void);
void func_02019390(void);
void func_0203bb60(void);
void func_0203bc7c(void);
void func_02023498(void);
void _ZN8dScene_c16SpawnIfNecessaryEv(void);
void func_02044120(void);
void func_02019404(void);
void _ZN3IRQ11DisableIRQsEj(unsigned int);
void func_020190b8(void);
void _ZN3IRQ10EnableIRQsEj(unsigned int);
void func_0201a4bc(void);
void func_0201ffcc(void);
void func_020132d8(void);
unsigned short func_0203da80(unsigned short);
int func_02030e4c(void);

void func_020197b8(void) {
    _ZN3G2x18SetBlendBrightnessEPVtts((void*)0x4000050, 0x3f, 0x1f);
    _ZN3G2x18SetBlendBrightnessEPVtts((void*)0x4001050, 0x3f, 0x1f);
    do {
        unsigned short v;
        data_0209d50c = 0xb; func_02019ac4();
        data_0209d50c = 2;   func_02019390();
        data_0209d50c = 0x16; func_0203bb60();
        data_0209d50c = 0x17; func_0203bc7c();
        v = *(unsigned short*)((char*)data_020a0e58 + (data_020a0e40 << 2));
        data_0209d534[data_0209d4e8] = v;
        data_0209d51c = v;
        data_0209d4e8 = (data_0209d4e8 + 1) & 0x1f;
        func_02023498();
        data_0209d50c = 3; _ZN8dScene_c16SpawnIfNecessaryEv();
        data_0209d50c = 4; func_02044120();
        data_0209d50c = 5; func_02019404();
        data_0209d50c = 6;
        data_020a0db0 = data_020a0db0 + 1;
        _ZN3IRQ11DisableIRQsEj(1);
        func_020190b8();
        data_0209d4f0 = 1;
        _ZN3IRQ10EnableIRQsEj(1);
        data_0209d50c = 7; func_0201a4bc();
        data_0209d4f0 = 0;
        data_0209d50c = 0x15; func_0201ffcc();
        if ((*(int*)(data_0209ee90 + 0x108) & 0x10) == 0)
            data_0209d514 = 0;
        data_0209d50c = 9;
        if (data_0209b480 != 0) func_020132d8();
        if (data_0209e64c != 0 || func_0203da80(0x9d) != 0)
            func_02030e4c();
    } while (1);
}
