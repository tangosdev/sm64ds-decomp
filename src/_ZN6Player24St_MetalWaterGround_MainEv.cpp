//cpp
extern "C" {
extern int func_ov002_020cec2c(void*);
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
extern void func_ov002_020cd190(void*);
extern void ApproachAngle(void*,short,int,int,int);
extern int func_ov002_020bf224(void*,int,int);
extern void _Z14ApproachLinearRiii(int*,int,int);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN6Player6IsAnimEj(void*,unsigned int);
extern int _ZN6Player12FinishedAnimEv(void*);
extern void func_ov002_020bf5e0(void*);
extern int _ZNK6Player14GetBodyModelIDEjb(void*,unsigned int,int);
extern int _ZNK9Animation12WillHitFrameEi(void*,int);
extern void func_0201251c(int,int,void*,int);
extern void func_ov002_020bedd4(void*);
extern int data_0209f32c;
extern unsigned char data_0209f49e;
extern unsigned char data_020a0e40;
extern short data_0209f4a0;
extern int data_ov002_0211013c[];
extern int data_ov002_021106c4[];
extern int data_ov002_0211067c[];

int _ZN6Player24St_MetalWaterGround_MainEv(char* c)
{
    if(func_ov002_020cec2c(c)) return 1;
    int u703 = *(unsigned char*)(c+0x703);
    if((int)(data_0209f32c - 0x50000) < (int)(*(int*)(c+0x60) - 0xa000)){
        *(unsigned char*)(c+0x706) = 0;
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
        return 1;
    }
    if(*(unsigned short*)((char*)&data_0209f49e + data_020a0e40 * 0x18) & 2){
        *(unsigned char*)(c+0x6e3) = 0;
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021106c4);
        return 1;
    }
    if(*(unsigned char*)(c+0x6de) != 0 && (*(int*)(c+0x60) - *(int*)(c+0x644)) >= 0x32000){
        *(unsigned char*)(c+0x6e3) = 1;
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021106c4);
        return 1;
    }
    if(*(unsigned char*)(c+0x6f9) == 0 && u703 == 0){
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211067c);
        return 1;
    }
    func_ov002_020cd190(c);
    int approach = 0;
    if(*(short*)((char*)&data_0209f4a0 + data_020a0e40 * 0x18) != 0){
        ApproachAngle((short*)(c+0x94), *(short*)(c+0x6d2), 8, 0x1000, 0x80);
        approach = func_ov002_020bf224(c, 0x14000, 0x1000);
    }
    _Z14ApproachLinearRiii((int*)(c+0x98), approach, 0x1000);
    *(short*)(c+0x8e) = *(short*)(c+0x94);
    if(*(unsigned char*)(c+0x703) != 0){
        if(*(int*)(c+0x98) == 0){
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0xa0, 0, 0x1000, 0);
        }else if(_ZN6Player6IsAnimEj(c, 0xa0) != 0){
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x9f, 0x40000000, 0x1000, 0);
        }else if(_ZN6Player6IsAnimEj(c, 0x9f) != 0){
            if(_ZN6Player12FinishedAnimEv(c) != 0){
                _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x9e, 0, *(short*)((char*)&data_0209f4a0 + data_020a0e40 * 0x18) * 2, 0);
            }
        }else if(_ZN6Player6IsAnimEj(c, 0x9f) == 0){
            int v = *(short*)((char*)&data_0209f4a0 + data_020a0e40 * 0x18);
            if(v != 0){
                _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x9e, 0, v * 2, 0);
                func_ov002_020bf5e0(c);
            }else{
                _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x9d, 0x40000000, 0x1000, 0);
            }
        }
    }else if(*(int*)(c+0x98) == 0){
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x47, 0, 0x1000, 0);
    }else{
        _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x48, 0, 0x1000, 0);
        int r5 = (int)((((long long)*(int*)(c+0x98) << 9) + 0x800) >> 12);
        if(r5 < 0x400) r5 = 0x400;
        char* anim = *(char**)(c + _ZNK6Player14GetBodyModelIDEjb(c, (unsigned char)*(int*)(c+8), 0) * 4 + 0xdc) + 0x50;
        *(int*)(anim + 0xc) = r5;
        if(_ZNK9Animation12WillHitFrameEi((void*)(*(char**)(c + _ZNK6Player14GetBodyModelIDEjb(c, (unsigned char)*(int*)(c+8), 0) * 4 + 0xdc) + 0x50), 4) != 0
           || _ZNK9Animation12WillHitFrameEi((void*)(*(char**)(c + _ZNK6Player14GetBodyModelIDEjb(c, (unsigned char)*(int*)(c+8), 0) * 4 + 0xdc) + 0x50), 0x13) != 0){
            func_0201251c(0, 0xaa, (void*)(c+0x74), *(int*)(c+0x98));
        }
    }
    func_ov002_020bedd4(c);
    return 1;
}

}
