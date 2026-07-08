//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef s32 Fix12;

extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int func_ov002_020bf56c(void* c, int b);
extern void ApproachAngle(s16* cur, s16 target, int divisor, int band, int maxStep);
extern Fix12 func_ov002_020bf30c(void* c, Fix12 a);
extern void _Z14ApproachLinearRiii(int* a, int b, int c);
extern int _ZN6Player6IsAnimEj(void* c, u32 a);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12 b, u32 d);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void func_ov002_020d3498(void* c);
extern void func_ov002_020bf88c(void* c);
extern int func_0201226c(int a0, int a1, int a2, int a3, int a4, s16 a5);
extern void func_ov002_020bedd4(void* c);

extern u16 data_0209f49e[];
extern u8 data_020a0e40;
extern s16 data_0209f4a0[];
extern int data_ov002_021101b4[];
extern int data_ov002_021101e4[];
extern int data_ov002_0211013c[];
}

extern "C" int _ZN6Player18St_TurnAround_MainEv(char* c)
{
    int r5v;
    s16 angle;
    int sel;
    int raw;
    int tmp;

    raw = *(s16*)(c + 0x6d2);
    if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
        angle = (s16)(raw + 0x8000);
        sel = (*(int*)(c + 0x98) < 0) ? 0x1800 : 0x8000;
        r5v = func_ov002_020bf56c(c, sel);
        ApproachAngle((s16*)(c + 0x94), angle, 0x10, 0x1000, 0x200);
        tmp = func_ov002_020bf30c(c, 0x28000);
        _Z14ApproachLinearRiii((int*)(c + 0x98), -tmp, r5v);
    } else {
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
        return 1;
    }

    ApproachAngle((s16*)(c + 0x8e), *(s16*)(c + 0x94), 8, 0x2000, 0x800);

    if ((*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 2) != 0) {
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021101e4);
    } else if (*(u8*)(c + 0x6de) != 0) {
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021101b4);
    } else if (*(int*)(c + 0x98) <= 0x12000) {
        if (!_ZN6Player6IsAnimEj(c, 0x45))
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x45, 0x40000000, 0x1000, 0);
        if (_ZN6Player12FinishedAnimEv(c)) {
            func_ov002_020d3498(c);
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
        }
    } else {
        func_ov002_020bf88c(c);
        *(int*)(c + 0x620) = func_0201226c(*(int*)(c + 0x620), 0, *(int*)(c + 0x66c) + 0xe2, (int)(c + 0x74), *(int*)(c + 0x98), 0);
    }

    func_ov002_020bedd4(c);
    return 1;
}
