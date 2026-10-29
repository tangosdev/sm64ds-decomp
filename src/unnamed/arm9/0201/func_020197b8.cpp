//cpp
#include "PlayerInput.h"
#include "G2x.h"
#include "IRQ.h"
#include "dScene_c.h"

extern "C" {

extern int data_0209d50c;
extern unsigned char data_0209d4e8;
extern unsigned short data_020a0e58[];
extern unsigned short data_0209d534[];
extern int data_0209d51c;
extern int data_020a0db0;
extern unsigned char data_0209d4f0;
extern unsigned char data_0209ee90[];
extern int data_0209d514;
extern unsigned char data_0209b480;
extern unsigned char data_0209e64c;

void func_02019ac4(void);
void func_02019390(void);
void func_0203bb60(void);
void func_0203bc7c(void);
void func_02023498(void);
void func_02044120(void);
void func_02019404(void);
void func_020190b8(void);
void func_0201a4bc(void);
void func_0201ffcc(void);
void func_020132d8(void);
unsigned short func_0203da80(unsigned short);
int func_02030e4c(void);

void func_020197b8(void) {
    G2x::SetBlendBrightness((volatile unsigned short *)0x4000050, 0x3f, 0x1f);
    G2x::SetBlendBrightness((volatile unsigned short *)0x4001050, 0x3f, 0x1f);
    do {
        unsigned short v;
        data_0209d50c = 0xb; func_02019ac4();
        data_0209d50c = 2;   func_02019390();
        data_0209d50c = 0x16; func_0203bb60();
        data_0209d50c = 0x17; func_0203bc7c();
        v = *(unsigned short*)((char*)data_020a0e58 + (gActivePlayerSlot << 2));
        data_0209d534[data_0209d4e8] = v;
        data_0209d51c = v;
        data_0209d4e8 = (data_0209d4e8 + 1) & 0x1f;
        func_02023498();
        data_0209d50c = 3;
        dScene_c::SpawnIfNecessary();
        data_0209d50c = 4; func_02044120();
        data_0209d50c = 5; func_02019404();
        data_0209d50c = 6;
        data_020a0db0 = data_020a0db0 + 1;
        IRQ::DisableIRQs(1);
        func_020190b8();
        data_0209d4f0 = 1;
        IRQ::EnableIRQs(1);
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
}
