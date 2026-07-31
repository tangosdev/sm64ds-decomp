typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

typedef struct { int x, y, z; } Vector3;

extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3j(u32 a, u32 b, u32 c, Vector3* v, u32 d);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 sysHandle, u32 kind, s32 scale, s32 p1, s32 p2, void* pos16, void* cb);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* self, void* state);
extern int func_ov002_020c5dec(char* c, int r1);
extern void Player_AdvanceAnims(char* self);

extern void* data_ov002_0211013c;
extern void* data_ov002_0211067c;

int _ZN6Player19St_Electrocute_MainEv(char *c)
{
    Vector3* pos = (Vector3*)(c+0x74);
    volatile s32 x;
    volatile s32 y;
    volatile s32 zvar;
    s32 z;
    s32 yraw;

    *(u32*)(c+0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(u32*)(c+0x620), 0, 0x10b, pos, 0);

    x = *(s32*)(c+0x5c);
    yraw = *(s32*)(c+0x60);
    y = yraw;
    {
        s32 zraw = *(s32*)(c+0x64);
        s32 yadjraw = yraw + 0x32000;
        z = zraw;
        zvar = zraw;
        y = yadjraw;
    }

    *(u32*)(c+0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(volatile u32*)(c+0x628), 0xc1, x, y, z, 0, 0);
    *(u32*)(c+0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(volatile u32*)(c+0x62c), 0xc2, x, y, zvar, 0, 0);
    *(u32*)(c+0x630) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(*(u32*)(c+0x630), 0xc3, x, y, zvar, 0, 0);

    if (*(u16*)(c+0x6a4) == 0) {
        u8 flags = *(u8*)(c+0x6e5);
        if ((flags & 1) == 0) {
            *(u16*)(c+0x6a0) = 0x24;
            flags = *(u8*)(c+0x6e5);
            if ((flags & 2) == 0) {
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
            } else {
                _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211067c);
            }
        } else {
            if ((flags & 2) != 0) {
                func_ov002_020c5dec(c, 8);
            } else {
                *(u16*)(c+0x6a0) = 0x24;
                if (func_ov002_020c5dec(c, 4) == 0) {
                    _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
                }
            }
        }
    }

    Player_AdvanceAnims(c);
    return 1;
}
