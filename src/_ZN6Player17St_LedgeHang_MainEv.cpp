//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef s32 Fix12;

extern "C" {
extern void func_ov002_020d0948(void* c);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12 b, u32 d);
extern int func_ov002_020cfaf0(void* c);
extern int func_ov002_020cfea4(void* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int func_ov002_020cfbdc(void* c);
extern int AngleDiff(int a, int b);
extern void func_ov002_020bedd4(void* c);

extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern s16 data_0209f4a0[];
extern int data_ov002_02110004[];
}

extern "C" int _ZN6Player17St_LedgeHang_MainEv(char* c)
{
    int v = *(int*)(c + 0x644);
    if (v != (int)0x80000000) {
        int d = v - *(int*)(c + 0x60);
        if (d < 0) d = -d;
        if (d < 0x28000) {
            *(int*)(c + 0x60) = v;
            *(u8*)(c + 0x6de) = 0;
            if (*(int*)(c + 0x558) < 0xddb) {
                func_ov002_020d0948(c);
                return 1;
            }
        }
    }

    if (*(u8*)(c + 0x6de) != 0) {
        func_ov002_020d0948(c);
        return 1;
    }

    if (*(u8*)(c + 0x6e3) == 0) {
        if (_ZN6Player12FinishedAnimEv(c)) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x21, 0, 0x1000, 0);
            *(u8*)(((int)c + 0x6e3) & 0xFFFFFFFFFFFFFFFF) =
                *(u8*)(((int)c + 0x6e3) & 0xFFFFFFFFFFFFFFFF) + 1;
        }
    } else {
        if ((!func_ov002_020cfaf0(c)
             && (*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 2))
            || func_ov002_020cfea4(c)) {
            *(u8*)(c + 0x6e3) = 1;
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_02110004);
        } else {
            if (func_ov002_020cfbdc(c))
                return 1;
            if (*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 0x400) {
                func_ov002_020d0948(c);
                return 1;
            }
            {
                if (*(u16*)(c + 0x6a4) == 0
                    && *(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
                    if (AngleDiff(*(s16*)(c + 0x6d2), *(s16*)(c + 0x8e)) >= 0x4000) {
                        func_ov002_020d0948(c);
                    } else if (!func_ov002_020cfaf0(c)) {
                        *(u8*)(c + 0x6e3) = 0;
                        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_02110004);
                    }
                }
            }
        }
    }

    func_ov002_020bedd4(c);
    return 1;
}
