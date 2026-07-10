//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef s32 Fix12;

extern "C" {
extern int _ZN6Player12FinishedAnimEv(void* c);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void* c, u32 anim, int a, Fix12 b, u32 d);
extern int func_ov002_020d1164(void* c);
extern int _ZNK6Player14GetBodyModelIDEjb(void* c, u32 a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void* thiz, int f);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 a, void* v);
extern int func_ov002_020d36d8(void* c, int arg);
extern int func_ov002_020bf30c(void* c, int a);
extern int func_ov002_020bf224(void* c, int a, int b);
extern void ApproachAngle(s16* cur, s16 target, int divisor, int band, int maxStep);
extern void func_ov002_020c18b0(void* c, int a);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern int func_ov002_020d4d88(void* c, int a, int b);
extern void func_ov002_020bedd4(void* c);

extern u8 data_020a0e40;
extern s16 data_0209f4a0[];
extern u16 data_0209f49c[];
extern int data_ov002_021104e4;
}

extern "C" int _ZN6Player13St_Crawl_MainEv(char* c)
{
    int a = 0;
    int b = 0;

    switch (*(u8*)(c + 0x6e3)) {
    case 3:
        if (_ZN6Player12FinishedAnimEv(c)) {
            *(u8*)(c + 0x6e3) = 4;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x63, 0, 0x1000, 0);
        }
        break;
    case 4: {
        u8 idx = data_020a0e40;
        int off = idx * 0x18;
        s16 val1 = *(s16*)((char*)data_0209f4a0 + off);
        if (val1 >= 0x200 && ((*(u16*)((char*)data_0209f49c + off) & 0x400) != 0 || func_ov002_020d1164(c) != 0)) {
            int id = _ZNK6Player14GetBodyModelIDEjb(c, *(int*)(c + 8) & 0xff, 0);
            char* anim = (char*)((int*)(c + 0xdc))[id] + 0x50;
            if (_ZNK9Animation12WillHitFrameEi(anim, 0xe) ||
                _ZNK9Animation12WillHitFrameEi((char*)((int*)(c + 0xdc))[_ZNK6Player14GetBodyModelIDEjb(c, *(int*)(c + 8) & 0xff, 0)] + 0x50, 0x1c)) {
                _ZN5Sound9PlayBank0EjRK7Vector3(*(u32*)(c + 0x66c) + 0xc0, c + 0x74);
            }
            int shifted = *(int*)(c + 0x98) >> 2;
            int id3 = _ZNK6Player14GetBodyModelIDEjb(c, *(int*)(c + 8) & 0xff, 0);
            char* anim3 = (char*)(((long long)(int)((char*)((int*)(c + 0xdc))[id3] + 0x50)) & 0xffffffffffffffffLL);
            *(int*)(anim3 + 0xc) = shifted;
            if (func_ov002_020d36d8(c, 1))
                return 1;
            int r4v = func_ov002_020bf30c(c, 0x8000);
            int r2v = func_ov002_020bf30c(c, 0x1000);
            a = func_ov002_020bf224(c, r4v, r2v);
            b = 0x1000;
            ApproachAngle((s16*)(c + 0x94), *(s16*)(c + 0x6d2), 0x10, 0x2000, 0x100);
            *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
            func_ov002_020c18b0(c, 0);
        } else {
            *(int*)(c + 0x98) = 0;
            *(u8*)(c + 0x6e3) = 5;
            _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x62, 0x40000000, 0x1000, 0);
        }
        break;
    }
    case 5:
        if (_ZN6Player12FinishedAnimEv(c)) {
            *(u8*)(c + 0x6e3) = 1;
            _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_021104e4);
        }
        break;
    }

    func_ov002_020d4d88(c, a, b);
    func_ov002_020bedd4(c);
    return 1;
}
