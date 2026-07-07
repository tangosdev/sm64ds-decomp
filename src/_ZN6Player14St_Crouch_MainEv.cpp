//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef s32 Fix12;

extern "C" {
extern void func_ov002_020bf90c(void* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int _ZN6Player6IsAnimEj(void* c, u32 a);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12 b, u32 d);
extern int func_ov002_020d12b0(void* c);
extern int func_ov002_020d1204(void* c);
extern int func_ov002_020d1164(void* c);
extern void func_ov002_020c06fc(void* c, u32 a);
extern int func_ov002_020dd2f4(void* c);
extern void func_ov002_020c0364(void* c, u32 a);
extern void func_ov002_020bedd4(void* c);

extern u16 data_0209f49c[];
extern u8 data_020a0e40;
extern s16 data_0209f4a0[];
extern int data_ov002_021101b4[];
extern int data_ov002_02110514[];
extern int data_ov002_0211013c[];
}

extern "C" int _ZN6Player14St_Crouch_MainEv(char* c)
{
    func_ov002_020bf90c(c);

    if (*(u8*)(c + 0x6de) != 0) {
        int d = *(int*)(c + 0x60) - *(int*)(c + 0x644);
        if (d >= 0x32000) {
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021101b4);
            return 1;
        }
    }

    switch (*(u8*)(c + 0x6e3)) {
    case 0:
        if (_ZN6Player6IsAnimEj(c, 0x2d) && _ZN6Player12FinishedAnimEv(c)) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x2c, 0, 0x1000, 0);
            *(u8*)(c + 0x6e3) = 1;
        }
        if (func_ov002_020d12b0(c))
            return 1;
        func_ov002_020d1204(c);
        break;
    case 1:
        if ((*(u16*)((char*)data_0209f49c + data_020a0e40 * 0x18) & 0x400) != 0
            || *(int*)(c + 0x98) != 0
            || func_ov002_020d1164(c) != 0) {
            if (*(int*)(c + 0x98) != 0) {
                func_ov002_020c06fc(c, 0x4000);
                func_ov002_020dd2f4(c);
            } else {
                if (*(u8*)(c + 0x703) == 0) {
                    if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) >= 0x200) {
                        _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_02110514);
                        return 1;
                    }
                }
            }
            u16 bit = *(u16*)(c + 0x6ce) & 1;
            if (bit != 0) {
                func_ov002_020c0364(c, 1);
                return 1;
            }
        } else {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x2e, 0x40000000, 0x1000, 0);
            *(u8*)(c + 0x6e3) = 2;
        }
        if (func_ov002_020d12b0(c))
            return 1;
        if (func_ov002_020d1204(c) != 0)
            return 1;
        break;
    case 2:
        if (_ZN6Player6IsAnimEj(c, 0x2e) && _ZN6Player12FinishedAnimEv(c)) {
            _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_0211013c);
            *(u16*)(c + 0x6b0) = 0xa;
        } else {
            func_ov002_020d1204(c);
        }
        break;
    }

    func_ov002_020bedd4(c);
    return 1;
}
