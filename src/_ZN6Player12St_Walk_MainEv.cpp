//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef s32 Fix12;

extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int func_ov002_020c5244(void);
extern int func_ov002_020d36d8(void* c, int a);
extern void func_ov002_020d45c0(void* c);
extern int func_ov002_020d3b9c(void* c);
extern void func_ov002_020d413c(void* c, short arg);
extern void ApproachAngle(short* cur, short target, int divisor, int band, int maxStep);
extern int _ZN6Player6IsAnimEj(void* c, u32 anim);
extern int func_0201226c(int a0, int a1, int a2, int a3, int a4, short a5);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12 b, u32 d);
extern void func_ov002_020d4540(void* c);
extern void func_ov002_020cabe0(void* c);
extern int _ZNK6Player14GetBodyModelIDEjb(void* c, u32 a, int b);
extern int _ZN9Animation8GetFlagsEv(void* anim);
extern void func_ov002_020d4748(void* c);
extern void func_ov002_020bedd4(void* c);

extern u8 data_020a0e40;
extern u8 data_0209f4ae[];
extern s16 data_0209f4a0[];
extern int data_ov002_02110154[];
}

extern "C" int _ZN6Player12St_Walk_MainEv(char* c)
{
    if ((u16)(*(u16*)(c + 0x6ce) & 0x800)) {
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_02110154);
        return 1;
    }
    if (func_ov002_020c5244()) return 1;
    if (func_ov002_020d36d8(c, 0)) return 1;

    func_ov002_020d45c0(c);
    short r5 = *(short*)(c + 0x94);
    if (func_ov002_020d3b9c(c)) return 1;

    func_ov002_020d413c(c, r5);

    if (*(int*)(c + 0x98) == 0) {
        *(short*)(c + 0x8e) = *(short*)(c + 0x94);
    } else {
        ApproachAngle((short*)(c + 0x8e), *(short*)(c + 0x94), 4, 0x2000, 0x800);
    }

    if (*(u8*)(c + 0x6e0) != 0) {
        if (!_ZN6Player6IsAnimEj(c, 0x37)) {
            int result = func_0201226c(*(int*)(c + 0x620), 0, *(int*)(c + 0x66c) + 0xe2, (int)(c + 0x74), *(int*)(c + 0x98), 0);
            *(int*)(c + 0x620) = result;
        }
        if (_ZN6Player12FinishedAnimEv(c)) {
            if (_ZN6Player6IsAnimEj(c, 0x36)) {
                if (*(int*)(c + 0x98) == 0) {
                    _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x37, 0x40000000, 0x1000, 0);
                }
            } else {
                if (_ZN6Player6IsAnimEj(c, 0x37)) {
                    func_ov002_020d4540(c);
                }
            }
        }
        goto end;
    }

    if (*(int*)(c + 0x98) != 0) goto label_29c;

    {
        u8 idx = data_020a0e40;
        int off = idx * 0x18;
        u8 b = data_0209f4ae[off];
        int thresh = (b != 2) ? 0x471 : 0x555;
        u8 v710 = *(u8*)(c + 0x710);
        int rhs = thresh - (v710 << 7);
        short lhs = *(s16*)((char*)data_0209f4a0 + off);
        if (lhs < rhs) goto label_1f0;
        if ((u16)(*(u16*)(c + 0x6ce) & 0xc) == 0) goto label_29c;
    }

label_1f0:
    if (*(u8*)(c + 0x703) != 0) {
        if (_ZN6Player12FinishedAnimEv(c) && _ZN6Player6IsAnimEj(c, 0x9d)) {
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_02110154);
            *(u8*)(c + 0x6e5) = 1;
            func_ov002_020cabe0(c);
        }
        goto end;
    }

    if (_ZN6Player12FinishedAnimEv(c)) goto label_27c;

    {
        u32 arg = (u8)*(int*)(c + 8);
        int modelIdx = _ZNK6Player14GetBodyModelIDEjb(c, arg, 0);
        char* anim = *(char**)(c + modelIdx * 4 + 0xdc) + 0x50;
        if (_ZN9Animation8GetFlagsEv(anim)) goto end;
    }

label_27c:
    _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_02110154);
    *(u8*)(c + 0x6e5) = 1;
    func_ov002_020cabe0(c);
    goto end;

label_29c:
    func_ov002_020d4748(c);
    *(u8*)(c + 0x6e5) = 1;

end:
    func_ov002_020bedd4(c);
    return 1;
}
