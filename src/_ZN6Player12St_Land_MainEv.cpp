//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef s32 Fix12;

extern "C" {
extern int func_ov002_020c0434(void* c);
extern void func_ov002_020c0364(void* c, u32 arg);
extern void func_ov002_020c06fc(void* c, u32 arg);
extern void func_ov002_020e04a4(void* c);
extern int func_ov002_020e3078(void* c, void* s);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int _ZN6Player6IsAnimEj(void* c, u32 a);
extern int func_ov002_020d5c6c(void* c);
extern int func_ov002_020dde74(void* c);
extern int func_ov002_020bf30c(void* c, int a);
extern int func_ov002_020bf224(void* c, int a, int b);
extern void _Z14ApproachLinearRiii(int* a, int b, int c);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12 b, u32 d);
extern int AngleDiff(int a, int b);
extern int _ZN6Player7IsStateERNS_5StateE(void* c, void* s);
extern void func_ov002_020bedd4(void* c);

extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern u16 data_0209f49c[];
extern s16 data_0209f4a0[];
extern int data_ov002_0211055c[];
extern int data_ov002_0211019c[];
extern int data_ov002_0211052c[];
extern int data_ov002_0211013c[];
}

extern "C" int _ZN6Player12St_Land_MainEv(char* c)
{
    if (func_ov002_020c0434(c)) {
        func_ov002_020c0364(c, 3);
        func_ov002_020c06fc(c, 0x4000);
        return 1;
    }

    func_ov002_020e04a4(c);
    u16 temp_r3 = *(u16*)(c + 0x6ce);

    if ((u16)(temp_r3 & 0x40) == 0) {
        if ((*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 2)
            || (u16)(temp_r3 & 0x100) != 0) {
            *(u16*)(((long long)(int)(c + 0x6ce)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x100;
            if (func_ov002_020e3078(c, data_ov002_0211055c) != 0
                && (*(u16*)((char*)data_0209f49c + data_020a0e40 * 0x18) & 0x400)
                && *(int*)(c + 0x98) >= 0) {
                _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211055c);
                return 1;
            }
            if (_ZN6Player6IsAnimEj(c, 0x4e) != 0) {
                *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
            }
            int v = (*(int*)(c + 0x358) != 0);
            if (v != 0) {
                *(s16*)(c + 0x6a8) = 0;
            }
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211019c);
            return 1;
        }
        if (func_ov002_020d5c6c(c) != 0) {
            return 1;
        }
        if (*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 1) {
            return func_ov002_020dde74(c);
        }
    } else {
        *(u16*)(((long long)(int)(c + 0x6ce)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x100;
    }

    _Z14ApproachLinearRiii((int*)(c + 0x98),
        func_ov002_020bf224(c, func_ov002_020bf30c(c, 0x20000), 0),
        0x4000);

    if (func_ov002_020e3078(c, data_ov002_0211052c) != 0) {
        *(int*)(c + 0x98) = 0;
    }

    if (*(u16*)(c + 0x6a4) != 0) {
        goto tail;
    }

    if ((*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0)
        || (_ZN6Player12FinishedAnimEv(c) != 0)) {
        if (_ZN6Player6IsAnimEj(c, 0x1b) != 0) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x2e, 0x40000000, 0x1000, 0);
            goto tail;
        }
        if (_ZN6Player6IsAnimEj(c, 0x4e) != 0) {
            *(s16*)(c + 0x8e) = (s16)(*(s16*)(c + 0x8e) + 0x8000);
        }
        if ((*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0)
            && AngleDiff(*(s16*)(c + 0x8e), *(s16*)(c + 0x94)) >= 0x4000) {
            if (AngleDiff(*(s16*)(c + 0x6d2), *(s16*)(c + 0x8e)) >= 0x4000) {
                *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
            } else {
                *(int*)(c + 0x98) = 0 - *(int*)(c + 0x98);
            }
        }
        *(s16*)(c + 0x94) = *(s16*)(c + 0x8e);
        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
        if (_ZN6Player7IsStateERNS_5StateE(c, data_ov002_0211013c) != 0
            && *(u8*)(c + 0x6e3) != 0
            && *(int*)(c + 0x98) >= func_ov002_020bf30c(c, 0x1c000)) {
            *(s16*)(c + 0x760) = 0x4000;
        }
        return 1;
    }

    if (func_ov002_020e3078(c, data_ov002_0211019c) == 0) {
        *(int*)(c + 0x98) = 0;
    }

tail:
    func_ov002_020bedd4(c);
    return 1;
}
