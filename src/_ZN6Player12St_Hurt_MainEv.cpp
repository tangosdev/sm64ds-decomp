//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

extern "C" {
extern void func_ov002_020d9a4c(char *c);
extern void func_ov002_020d99a4(unsigned char *self);
extern void func_ov002_020d9aac(char* c);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int a, void* v);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void* v);
extern void func_ov002_020d94cc(char *self);
extern void func_ov002_020bf9d4(char* c);
extern void func_ov002_020d98b4(char* self);
extern int _ZN6Player12FinishedAnimEv(char* c);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* s);
extern void Player_AdvanceAnims(char* c);
extern int func_ov002_020e2c84(char *self);

extern int data_ov002_0211067c;
extern int data_ov002_0211013c;
}

extern "C" int _ZN6Player12St_Hurt_MainEv(char* c)
{
    *(u8*)(c + 0x713) = 1;
    if (*(u16*)(c + 0x6a8) != 0) {
        *(u8*)(c + 0x713) = 0;
    }

    switch (*(u8*)(c + 0x6e6)) {
    case 0:
        func_ov002_020d9a4c(c);
        func_ov002_020d99a4((unsigned char*)c);
        if (*(u8*)(c + 0x6de) != 0) break;
        func_ov002_020d9aac(c);
        *(u16*)(c + 0x6a4) = 0;
        if ((*(u8*)(c + 0x6e3) & 7) >= 2) {
            _ZN5Sound9PlayBank0EjRK7Vector3(*(u32*)(c + 0x66c) + 0x50, c + 0x74);
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8*)(c + 0x6d9), 0x11, c + 0x74);
        }
        if (*(s32*)(c + 0x674) != 0) {
            u32 masked = *(u8*)(c + 0x6e3) & 0xf0;
            if (masked == 0 || masked == 0x10) {
                func_ov002_020d94cc(c);
            }
            *(s32*)(c + 0x674) = 0;
        }
        if (*(u8*)(c + 0x707) == 0) {
            func_ov002_020bf9d4(c);
        }
        *(u8*)(c + 0x6e6) = 1;
        break;

    case 1:
        func_ov002_020d9a4c(c);
        func_ov002_020d99a4((unsigned char*)c);
        if (*(u8*)(c + 0x6de) != 0) break;
        func_ov002_020d9aac(c);
        if (*(s32*)(c + 0x98) != 0) break;
        *(u8*)(c + 0x6e6) = 2;
        if ((*(u8*)(c + 0x6e5) >> 4) == 2) {
            *(u16*)(c + 0x6a4) = 0x5a;
        }
        break;

    case 2:
        if (*(u16*)(c + 0x6a4) != 0) {
            func_ov002_020d98b4(c);
            return 1;
        }
        *(s32*)(((long long)(int)(c + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x80;
        *(unsigned char*)(((long long)(int)(c + 0x6e5)) & 0xFFFFFFFFFFFFFFFFLL) &= 0xf0;
        if (_ZN6Player12FinishedAnimEv(c)) {
            if ((*(u8*)(c + 0x6e3) & 1) == 0) {
                *(s16*)(c + 0x94) = *(s16*)(c + 0x94) + 0x8000;
                *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
            }
            *(u8*)(c + 0x708) = 0;
            if (*(u8*)(c + 0x70c) & 2) {
                *(u16*)(c + 0x6a0) = 0x24;
            }
            if (*(u8*)(c + 0x706) != 0) {
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211067c);
            } else {
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
            }
        }
        break;

    default:
        break;
    }

    if ((*(u8*)(c + 0x6e5) & 0xf) == 0) {
        Player_AdvanceAnims(c);
    }
    *(s32*)(c + 0x640) = *(s32*)(c + 0xa8);
    if (*(u8*)(c + 0x6de) == 0) {
        func_ov002_020e2c84(c);
    }
    return 1;
}
