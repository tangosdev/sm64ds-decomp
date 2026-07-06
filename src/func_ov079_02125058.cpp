//cpp
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct { s32 x, y, z; } Vector3;
typedef s32 Fix12;

extern "C" {
extern int func_ov002_020dd8b8(void* c);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void* self, const Vector3* v, u32 n, Fix12 f, short s);
}

extern "C" void func_ov079_02125058(char* c, char* a)
{
    volatile Vector3 v1, v2;
    Vector3 w1, w2;
    int eq = (int)(*(u16*)(a + 0xc) == 0xbf);
    if (!eq) return;
    *(u8*)(c + 0x403) = 1;
    if (func_ov002_020dd8b8(a) != 0) {
        if (*(u8*)(c + 0x40b) != 0 && *(u8*)(c + 0x414) != 0) {
            if (*(s32*)(c + 0x3b4) == 0) return;
        }
        *(u8*)(c + 0x402) = 1;
        *(void**)(c + 0x3ac) = a;
        return;
    }
    if (*(u8*)(c + 0x405) != 0) return;
    if (*(u8*)(c + 0x414) == 0) {
        Vector3* p;
        s32 x, y, z;
        if (DecIfAbove0_Byte((u8*)(c + 0x406)) == 0) return;
        p = (Vector3*)(((long long)(int)(a + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        x = p->x; v1.x = x;
        y = p->y; v1.y = y;
        z = p->z; v1.z = z;
        w1.x = x; w1.y = y; w1.z = z;
        _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &w1, 1, 0x8000, 0);
        *(u8*)(c + 0x405) = 1;
        return;
    }
    {
        Vector3* p;
        s32 x, y, z;
        u32 t = *(s32*)(c + 0x3b0);
        if (t - 1 > 1) return;
        p = (Vector3*)(((long long)(int)(a + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        x = p->x; v2.x = x;
        y = p->y; v2.y = y;
        z = p->z; v2.z = z;
        if (y - *(s32*)(c + 0x60) < 0x2bc000) return;
        if (DecIfAbove0_Byte((u8*)(c + 0x406)) == 0) return;
        w2.x = v2.x; w2.y = v2.y; w2.z = v2.z;
        _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &w2, 1, 0x8000, 0);
        *(u8*)(c + 0x405) = 1;
        return;
    }
}
