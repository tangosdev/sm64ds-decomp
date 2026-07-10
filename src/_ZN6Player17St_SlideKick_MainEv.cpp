//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef s32 Fix12;

extern "C" {
extern void func_ov002_020bf90c(char* c);
extern void func_ov002_020c06fc(char* c, u32 flag);
extern int func_ov002_020dd2f4(char* c);
extern int _ZN6Player12FinishedAnimEv(void* c);
extern int _ZN6Player6IsAnimEj(void* c, u32 a);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12 b, u32 d);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int func_ov002_020c0688(char* c);
extern int _ZNK6Player14GetBodyModelIDEjb(void* c, u32 a, int b);
extern void func_ov002_020dbaec(char* c);
extern void _ZN12CylinderClsn5ClearEv(void* c);
extern void _ZN12CylinderClsn6UpdateEv(void* c);
extern void func_ov002_020bedd4(char* c);

extern void* data_ov002_021104e4;
extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern void* data_ov002_021101fc;
extern void* data_ov002_021101b4;
}

extern "C" int _ZN6Player17St_SlideKick_MainEv(char* c)
{
    func_ov002_020bf90c(c);

    if (*(u8*)(c + 0x6de) == 0) {
        *(u8*)(c + 0x6e4) = 1;
        func_ov002_020c06fc(c, 0x4000);
        func_ov002_020dd2f4(c);
    }

    if (*(int*)(c + 0x98) == 0) {
        if (_ZN6Player12FinishedAnimEv(c)) {
            if (_ZN6Player6IsAnimEj(c, 0x67)) {
                _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x66, 0x40000000, 0x1000, 0);
            } else {
                *(s16*)(c + 0x94) = *(s16*)(c + 0x8e);
                *(u8*)(c + 0x6e3) = 1;
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021104e4);
            }
        }
    } else {
        if (func_ov002_020c0688(c)) {
            if (*(u8*)(c + 0x6e5) == 0) {
                if (*(int*)(c + 0x640) < 0) {
                    u16 flag = (u16)(*(u16*)(c + 0x6ce) & 1);
                    if (flag == 0) {
                        int r0 = -*(int*)(c + 0x640);
                        if (r0 > 0x14000) r0 = 0x14000;
                        *(int*)(c + 0xa8) = r0;
                        *(u8*)(c + 0x6e5) = 1;
                    }
                }
            }

            {
                u16 val = *(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18);
                if ((val & 1) || (val & 2)) {
                    if (*(u8*)(c + 0x70e) == 0) {
                        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021101fc);
                    }
                }
                *(u8*)(c + 0x6e3) = 0;
            }
        } else {
            *(u8*)(((int)c + 0x6e3) & 0xFFFFFFFFFFFFFFFF) =
                *(u8*)(((int)c + 0x6e3) & 0xFFFFFFFFFFFFFFFF) + 1;
            if (*(u8*)(c + 0x6e3) > 0x1e) {
                if (*(int*)(c + 0x60) - *(int*)(c + 0x644) > 0x64000) {
                    _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021101b4);
                }
            }
        }

        {
            int id = _ZNK6Player14GetBodyModelIDEjb(c, *(u32*)(c + 8) & 0xff, 0);
            void* anim = *(void**)(c + (id << 2) + 0xdc);
            char* anim50 = (char*)(((int)anim + 0x50) & 0xFFFFFFFFFFFFFFFF);
            u32 v = *(u32*)(anim50 + 8);
            u16 t = (u16)(v >> 12);
            if (t >= 4) {
                func_ov002_020dbaec(c);
                _ZN12CylinderClsn5ClearEv(c + 0x314);
                _ZN12CylinderClsn6UpdateEv(c + 0x314);
            }
        }
    }

    func_ov002_020bedd4(c);
    *(int*)(c + 0x640) = *(int*)(c + 0xa8);
    return 1;
}
