//cpp
typedef int s32;
typedef unsigned int u32;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned char u8;

struct Vector3 { int x, y, z; };

extern "C" {
int _ZNK12WithMeshClsn8IsOnWallEv(void* self);
void _ZN5Actor10PoofDustAtERK7Vector3(void* self, const struct Vector3* v);
void _ZN5Actor8PoofDustEv(void* self);
void* _ZN5Actor10FindWithIDEj(u32 id);
void func_ov063_02116244(char* c);
}

extern "C" int func_ov063_0211a3d0(char* c)
{
    struct Vector3 v, w;

    if (*(u16*)(c + 0x100) == 0) {
        *(s32*)(c + 0x98) = 0x28000;
        *(s16*)(c + 0x94) = *(s16*)(*(char**)(c + 0x484) + 0x8e);
        *(s32*)(c + 0x5a0) = 1;
        *(u16*)(((int)c + 0x5d4) & 0xFFFFFFFFFFFFFFFFLL) &= ~4;
        goto fall;
    }

    if (*(u16*)(c + 0x100) == 5)
        *(u8*)(c + 0x5c9) = 8;

    if (*(u16*)(c + 0x100) <= 0x1e) {
        if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x1c4) == 0)
            goto fall;
    }

    if (*(u8*)(c + 0x5cf) == 0xf) {
        int x = *(s32*)(c + 0x5c);
        v.x = x;
        v.y = *(s32*)(c + 0x60);
        v.z = *(s32*)(c + 0x64);
        if ((u32)(*(u16*)(c + 0x5d4) << 23) >> 31)
            v.x = x * (u32)-1;
        ((int*)&w)[0] = ((int*)&v)[0];
        ((int*)&w)[1] = ((int*)&v)[1];
        ((int*)&w)[2] = ((int*)&v)[2];
        _ZN5Actor10PoofDustAtERK7Vector3(c, &w);
    } else {
        _ZN5Actor8PoofDustEv(c);
    }

    *(s32*)(c + 0x5a0) = 2;

    if (*(u8*)(c + 0x5cf) != 0 && *(u32*)(c + 0x490) != 0) {
        *(void**)(c + 0x48c) = _ZN5Actor10FindWithIDEj(*(u32*)(c + 0x490));
        if (*(void**)(c + 0x48c) != 0) {
            if (*(u8*)(c + 0x5cf) != 2) {
                *(s32*)(((int)*(char**)(c + 0x48c) + 0x180) & 0xFFFFFFFFFFFFFFFFLL) += 1;
            }
            func_ov063_02116244(*(char**)(c + 0x48c));
        }
        *(void**)(c + 0x48c) = 0;
    }
    return 1;

fall:
    *(s32*)(c + 0xa8) = 0x5000;
    *(s16*)(((int)c + 0x90) & 0xFFFFFFFFFFFFFFFFLL) += 0x800;
    *(s16*)(((int)c + 0x8e) & 0xFFFFFFFFFFFFFFFFLL) += 0x800;
    return 0;
}
