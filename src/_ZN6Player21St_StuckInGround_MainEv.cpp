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
extern int _ZNK6Player14GetBodyModelIDEjb(void* c, u32 a, int b);
extern int _ZNK9Animation12WillHitFrameEi(void* a, int f);
extern void func_ov002_020c5444(char* c);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 id, void* v);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, void* s);
extern void func_ov002_020bedd4(char* c);

extern u32 data_ov002_0210a578[];
extern u32 data_ov002_0210a584[];
extern u8 data_020a0e40;
extern u16 data_0209f49e[];
extern u8 data_ov002_020ff0ec[];
extern int _ZN6Player7ST_WALKE[];
}

extern "C" int _ZN6Player21St_StuckInGround_MainEv(char* c)
{
    switch (*(u8*)(c + 0x6e5)) {
    case 0:
        if (_ZN6Player12FinishedAnimEv(c)) {
            _ZN6Player7SetAnimEji5Fix12IiEj(c, data_ov002_0210a578[*(u8*)(c + 0x6e3)], 0x40000000, 0x1000, 0);
            *(u8*)(((int)c + 0x6e5) & 0xFFFFFFFFFFFFFFFF) =
                *(u8*)(((int)c + 0x6e5) & 0xFFFFFFFFFFFFFFFF) + 1;
            return 1;
        }
        break;
    case 1:
        if (_ZN6Player12FinishedAnimEv(c) == 0) {
            if ((*(u16*)((char*)data_0209f49e + data_020a0e40 * 0x18) & 2) == 0)
                break;
        }
        _ZN6Player7SetAnimEji5Fix12IiEj(c, data_ov002_0210a584[*(u8*)(c + 0x6e3)], 0x40000000, 0x1000, 0);
        *(u8*)(((int)c + 0x6e5) & 0xFFFFFFFFFFFFFFFF) =
            *(u8*)(((int)c + 0x6e5) & 0xFFFFFFFFFFFFFFFF) + 1;
        break;
    case 2:
        {
            void* anim = *(void**)(c + (_ZNK6Player14GetBodyModelIDEjb(c, *(u32*)(c + 8) & 0xff, 0) << 2) + 0xdc);
            if (_ZNK9Animation12WillHitFrameEi((char*)anim + 0x50, data_ov002_020ff0ec[*(u8*)(c + 0x6e3)])) {
                func_ov002_020c5444(c);
                _ZN5Sound9PlayBank0EjRK7Vector3(0xb4, c + 0x74);
            }
        }
        if (_ZN6Player12FinishedAnimEv(c)) {
            *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
            _ZN6Player11ChangeStateERNS_5StateE(c, _ZN6Player7ST_WALKE);
        }
        break;
    }

    func_ov002_020bedd4(c);
    return 1;
}
