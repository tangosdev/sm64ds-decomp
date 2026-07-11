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
extern int _ZN6Player6IsAnimEj(void* c, u32 anim);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern int _ZNK6Player14GetBodyModelIDEjb(void* c, u32 a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void* anim, int frame);
extern void _Z14ApproachLinearRiii(int* a, int b, int c);
extern void func_ov002_020da9d4(void* c);
extern void ApproachAngle(short* cur, short target, int divisor, int band, int maxStep);
extern int func_ov002_020bf224(void* c, int a, int b);
extern void func_ov002_020d4d88(void* c, int a, int b);
extern int _ZN9Animation8GetFlagsEv(void* anim);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12 b, u32 d);
extern void func_ov002_020d1f78(void* c, u32 a);
extern void func_ov002_020bedd4(void* c);

extern int data_ov002_0211013c[];
extern int data_ov002_020ff254[];
extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern int data_ov002_021105d4[];
extern int data_ov002_021101b4[];
extern s16 data_0209f4a0[];
extern int data_ov002_020ff1d0[];
extern u8 data_0209f4ac[];
extern u16 data_0209f49c[];
}

extern "C" int _ZN6Player17St_HoldHeavy_MainEv(char* c)
{
    int var_r1 = 0;

    if (*(int*)(c + 0x358) == 0) {
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
        return 1;
    }

    if (*(u8*)(c + 0x6e3) == 0) {
        u32 anim = data_ov002_020ff254[*(u8*)(c + 0x6e5)];
        if (_ZN6Player6IsAnimEj(c, anim)) {
            if (_ZN6Player12FinishedAnimEv(c)) {
                *(u8*)(c + 0x6e3) = 1;
            } else {
                u32 arg = (u8)*(int*)(c + 8);
                int modelIdx = _ZNK6Player14GetBodyModelIDEjb(c, arg, 0);
                char* animPtr = *(char**)(c + modelIdx * 4 + 0xdc) + 0x50;
                if (_ZNK9Animation12WillHitFrameEi(animPtr, 6)) {
                    int* heavy = *(int**)(c + 0x358);
                    if (heavy != 0) {
                        *(int*)(((long long)(int)((char*)heavy + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x4000;
                    }
                }
            }
        }
        _Z14ApproachLinearRiii((int*)(c + 0x98), 0, 0x1000);
        goto end;
    }

    {
        int off = data_020a0e40 * 0x18;
        u16 flags = *(u16*)((char*)data_0209f49e + off);
        if (flags & 1) {
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021105d4);
            return 1;
        }

        if (*(u8*)(c + 0x6de) != 0) {
            func_ov002_020da9d4(c);
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021101b4);
            *(u16*)(c + 0x6b4) = 0xa;
            return 1;
        }

        {
            s16 t = *(s16*)((char*)data_0209f4a0 + off);
            if (t != 0) {
                ApproachAngle((short*)(c + 0x94), *(s16*)(c + 0x6d2), 8, 0x1000, 0x400);
                var_r1 = data_ov002_020ff1d0[*(int*)(c + 8)];
            }
        }
    }

    {
        int off2 = data_020a0e40 * 0x18;
        if (*((u8*)data_0209f4ac + off2) == 0) {
            if (!(*(u16*)((char*)data_0209f49c + off2) & 0x800)) {
                var_r1 >>= 1;
            }
        } else {
            var_r1 = func_ov002_020bf224(c, var_r1, 0);
        }
        func_ov002_020d4d88(c, var_r1, 0x1000);
    }

    if (*(int*)(c + 0x98) == 0) {
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
    } else if (*(int*)(c + 8) == 1) {
        ApproachAngle((short*)(c + 0x8e), *(s16*)(c + 0x94), 0x10, 0x1000, 0x100);
    } else {
        ApproachAngle((short*)(c + 0x8e), *(s16*)(c + 0x94), 8, 0x2000, 0x400);
    }

    if (*(int*)(c + 0x98) == 0) {
        if (_ZN6Player12FinishedAnimEv(c) ||
            !_ZN9Animation8GetFlagsEv(
                *(char**)(c + _ZNK6Player14GetBodyModelIDEjb(c, (u8)*(int*)(c + 8), 0) * 4 + 0xdc) + 0x50)) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, data_ov002_020ff254[*(u8*)(c + 0x6e5) + 2], 0, 0x1000, 0);
        }
    } else {
        func_ov002_020d1f78(c, data_ov002_020ff254[*(u8*)(c + 0x6e5) + 4]);
    }

end:
    func_ov002_020bedd4(c);
    return 1;
}
