//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef long long s64;
typedef s32 Fix12;

extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int func_ov002_020c0434(void* c);
extern void func_ov002_020c0364(void* c, u32 arg);
extern void _Z14ApproachLinearRiii(int* a, int b, int c);
extern int _ZN6Player6IsAnimEj(void* c, u32 anim);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern int func_ov002_020e0ccc(void* c, short* st);
extern int _ZNK6Player14GetBodyModelIDEjb(void* c, u32 a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void* anim, int frame);
extern int func_ov002_020c5244(void* c);
extern int func_ov002_020dab14(void* c);
extern void ApproachAngle(short* cur, short target, int divisor, int band, int maxStep);
extern int func_ov002_020bf224(void* c, int a, int b);
extern void func_ov002_020d4d88(void* c, int a, int b);
extern int func_ov002_020d9dcc(void* c);
extern int _ZN9Animation8GetFlagsEv(void* anim);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12 b, u32 d);
extern void func_ov002_020d1f78(void* c, u32 a);
extern void func_ov002_020bedd4(void* c);

extern u8 data_020a0e40;
extern s16 data_0209f4a0[];
extern int data_ov002_020ff1c0[];
extern u8 data_0209f4ac[];
extern u16 data_0209f49c[];
extern u16 data_0209f49e[];
extern int data_ov002_0211013c[];
extern int data_ov002_021105d4[];
extern int data_ov002_0211019c[];
extern int data_ov002_021101b4[];
}

extern "C" int _ZN6Player17St_HoldLight_MainEv(char* c)
{
    if (*(int*)(c + 0x358) == 0) {
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
        return 1;
    }
    if (func_ov002_020c0434(c)) {
        func_ov002_020c0364(c, 3);
        return 1;
    }

    if (*(u8*)(c + 0x6e3) == 0) {
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0, 0x800);
        if (_ZN6Player6IsAnimEj(c, 0x2f) || _ZN6Player6IsAnimEj(c, 0x86)) {
            if (_ZN6Player12FinishedAnimEv(c)) {
                if (func_ov002_020e0ccc(c, *(short**)(c + 0x358))) {
                    return 1;
                }
                *(s16*)(c + 0x94) = *(s16*)(c + 0x8e);
                *(int*)(c + 0x98) = 0;
                *(u8*)(c + 0x6e3) = 1;
            } else {
                u32 arg = (u8)*(int*)(c + 8);
                int modelIdx = _ZNK6Player14GetBodyModelIDEjb(c, arg, 0);
                char* anim = *(char**)(c + modelIdx * 4 + 0xdc) + 0x50;
                if (_ZNK9Animation12WillHitFrameEi(anim, 6)) {
                    int* light = *(int**)(c + 0x358);
                    if (light != 0) {
                        int* p = (int*)(((long long)(int)((char*)light + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL);
                        *p |= 0x4000;
                    }
                }
            }
        }
        goto end;
    }

    if (func_ov002_020c5244(c)) {
        return 1;
    }

    if (*(int*)(c + 0x68c) > 0x19000) {
        func_ov002_020dab14(c);
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
        return 1;
    }

    int var_r4 = 0;
    if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
        s16 t = *(s16*)(c + 0x6d2);
        if (*(u8*)(c + 0x70c) != 0) {
            *(s16*)(c + 0x94) = t;
            *(u8*)(c + 0x70c) = 0;
        }
        ApproachAngle((short*)(c + 0x94), t, 4, 0x2000, 0x800);
        var_r4 = data_ov002_020ff1c0[*(int*)(c + 8)];
    }

    if (*((u8*)data_0209f4ac + data_020a0e40 * 0x18) == 0) {
        if (!(*(u16*)((char*)data_0209f49c + data_020a0e40 * 0x18) & 0x800)) {
            var_r4 = (s32)(((s64)var_r4 * 0xa00 + 0x800) >> 0xc);
        }
    } else {
        var_r4 = func_ov002_020bf224(c, var_r4, 0);
    }

    *(u8*)(c + 0x70c) = 0;
    if (*(int*)(c + 0x98) == 0) {
        *(u8*)(c + 0x70c) = 1;
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
    } else {
        ApproachAngle((short*)(c + 0x8e), *(s16*)(c + 0x94), 8, 0x2000, 0x800);
    }

    func_ov002_020d4d88(c, var_r4, 0x1000);

    {
        u16 flags = *(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18);
        if (flags & 1) {
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021105d4);
            return 1;
        }
        if (flags & 2) {
            *(s16*)(c + 0x6a8) = 0;
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211019c);
            return 1;
        }
        if (flags & 0x400) {
            if (func_ov002_020dab14(c)) {
                _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
            }
            return 1;
        }
    }

    if (*(u8*)(c + 0x6de) != 0) {
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021101b4);
        return 1;
    }
    if (func_ov002_020d9dcc(c)) {
        return 1;
    }

    if (*(int*)(c + 0x98) == 0) {
        if (_ZN6Player12FinishedAnimEv(c) ||
            !_ZN9Animation8GetFlagsEv(
                *(char**)(c + _ZNK6Player14GetBodyModelIDEjb(c, (u8)*(int*)(c + 8), 0) * 4 + 0xdc) + 0x50)) {
            int* light = *(int**)(c + 0x358);
            u32 animId = 0x33;
            if (light != 0) {
                int b = (*(int*)((char*)light + 0xb0) & 0x8000) != 0;
                if (b) {
                    animId = 0x87;
                    if (*(int*)(c + 8) == 2) {
                        int b2 = (*(u16*)((char*)light + 0xc) == 0xce);
                        if (b2) {
                            animId = 0x33;
                        }
                    }
                }
            }
            _ZN6Player7SetAnimEji5Fix12IiEj(c, animId, 0, 0x1000, 0);
        }
    } else {
        func_ov002_020d1f78(c, 0x48);
    }

end:
    func_ov002_020bedd4(c);
    return 1;
}
