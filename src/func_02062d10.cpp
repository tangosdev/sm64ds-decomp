//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern "C" void *func_02061548(void);

struct CP15 { static void InvalidateDataCache(u32 addr, u32 len); };

struct Ctx {
    char pad0[4];
    char *p4;       /* 0x4 */
};

struct In {
    char pad0[2];
    u16 u2;         /* 0x2 */
    char pad1[0xa - 0x4];
    u16 ua;         /* 0xa */
    char pad2[0x1c - 0xc];
    u16 u1c;        /* 0x1c */
};

extern "C" void func_02062d10(In *self) {
    Ctx *c = (Ctx*)func_02061548();
    char *r4 = *(char**)((char*)c + (self->ua << 2) + 0xec);
    CP15::InvalidateDataCache((u32)(c->p4 + 0x184), 2);
    u16 v = *(u16*)(c->p4 + 0x100 + 0x84);
    int kind = self->u2;
    if (kind == 0) {
        if (v != 0) return;
        u16 *g = (u16*)(r4 + 0x400);
        *(u16*)(r4 + (g[5] << 1) + 0x400) = (u16)(self->u1c >> 1);
        g[5] = (u16)((g[5] + 1) % 4);
    } else if (kind == 0xa) {
        if (v != 0) {
            u16 *g = (u16*)(r4 + 0x400);
            g[5] = (u16)((g[5] + 3) % 4);
        }
        u16 *g = (u16*)(r4 + 0x400);
        *(u16*)(g + 0x1c/2) = 2;
    } else {
        *(u16*)(r4 + 0x400 + 0x1c) = 3;
    }
}
