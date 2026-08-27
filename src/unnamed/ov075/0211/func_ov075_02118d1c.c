#include "types.h"
extern int func_0203d974(void);
extern u8 func_020202dc(void);
extern u8 func_02020168(void);
extern void func_02020304(void);
extern void func_020200e0(void);
extern u8 func_020200b8(void);
extern void func_ov001_020ab3f0(void* r0);
extern void func_ov075_0211a194(void* c, void* d);
extern void func_02012790(u32 a);
extern int func_0203d9b4(void);
extern int func_ov075_02116d40(void* c);
extern u8 TouchArea_Update(void* p, int i);
extern void func_02020078(u32 flags);
extern void func_02020124(void);
extern void func_0202029c(u32 idx);
extern int func_0203da9c(void);
extern u8 data_0209b2e4;
extern u8 data_ov075_0211d7b0[];
extern u8 data_ov075_0211d798[];
extern u8 data_ov075_0211d7f8[];
extern u8 data_020a0e40;
extern u16 data_020a0e5a[];

void func_ov075_02118d1c(char* c){
    if(func_0203d974() && func_020202dc() != data_0209b2e4){
        data_0209b2e4 = func_020202dc();
    }
    if(func_02020168()){
        u8 st;
        func_02020304();
        func_020200e0();
        st=func_020200b8();
        if(st==0){
            int r5;
            char* r3;
            u8 v;
            func_ov001_020ab3f0(c+0x70 + data_0209b2e4*0x24);
            *(u8*)(c + data_0209b2e4*0x24 + 0x81) = 0;
            r3=c;
            r5=0;
            v=r5;
            for(; r5<4; r5++){
                if(r5 != data_0209b2e4) *(u8*)(r3+0x80)=v;
                r3+=0x24;
            }
            func_ov075_0211a194(c, data_ov075_0211d7b0);
            func_02012790(0xa0);
            func_02012790(0x128);
            return;
        }
        if(st==1){
            func_ov075_0211a194(c, data_ov075_0211d798);
            func_02012790(0x9e);
            return;
        }
        func_ov075_0211a194(c, data_ov075_0211d7f8);
        func_02012790(0x9f);
        return;
    }
    if(func_0203d9b4()){
        if(func_ov075_02116d40(c)){
            int r6, r5;
            r6=0;
            r5=r6;
            c+=0x70;
            for(; r5<4; r5++){
                if(TouchArea_Update(c, r6)){
                    r6=1;
                    data_0209b2e4=r5;
                    break;
                }
                c+=0x24;
            }
            if(r6){
                func_02020078(0);
                func_02020124();
            } else {
                if(data_020a0e5a[data_020a0e40*2]&4){
                    func_02020078(1);
                    func_02020124();
                } else if(data_020a0e5a[data_020a0e40*2]&8){
                    func_02020078(2);
                    func_02020124();
                }
            }
            func_0202029c(data_0209b2e4);
        }
    }
    *(u16*)0x4000050 = 0;
    if(data_020a0e5a[func_0203da9c()*2] & ~0xc){
        func_02012790(0xe);
    }
}
