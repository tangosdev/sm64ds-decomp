typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;

struct Vector3 { int x, y, z; };

extern signed char LEVEL_ID;
extern void* _ZN5Actor13ClosestPlayerEv(void* c);
extern int Vec3_HorzDist(const struct Vector3* a, const struct Vector3* b);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(u32 a, u32 b, const struct Vector3* p, const void* q, int e, int f);

void func_ov100_02142b90(char* c)
{
    struct Vector3 pos;
    u16* timer = (u16*)(c + 0x100);
    void* pl;
    int r1;

    if (*timer != 0) {
        *timer = *timer - 1;
        return;
    }

    if (LEVEL_ID == 6) r1 = 3; else r1 = 6;
    if (*(u8*)(c + 0x3d2) >= (u32)r1) return;

    pl = _ZN5Actor13ClosestPlayerEv(c);
    if (pl == 0) return;

    {
        struct Vector3* pp = (struct Vector3*)(((long long)(int)((char*)pl + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        pos.x = pp->x;
        pos.y = pp->y;
        pos.z = pp->z;
    }

    if ((*(int*)(c + 8) & 0xf) != 4) {
        if (pos.y >= *(int*)(c + 0x60) - 0x28000) return;
    }

    {
        int d = Vec3_HorzDist((struct Vector3*)(c + 0x5c), &pos);
        if (d < *(int*)(c + 0x3c0)) return;
        if (d > *(int*)(c + 0x3c4)) return;
    }

    if (LEVEL_ID == 0x16) {
        if (pos.y < (int)0xff63c000) return;
        *(u16*)(c + 0x100) = 0x3f;
    } else {
        *(u16*)(c + 0x100) = 0x7f;
    }

    {
        void* a;
        int cc = *(signed char*)(c + 0xcc);
        a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xdc, *(int*)(c + 8),
            (struct Vector3*)(c + 0x5c), (const void*)(c + 0x92), cc, -1);
        if (a == 0) return;
        {
            u8* cnt = (u8*)(((long long)(int)(c + 0x3d2)) & 0xFFFFFFFFFFFFFFFFLL);
            *cnt = *cnt + 1;
        }
        *(int*)((char*)a + 0x3a8) = (int)c;
    }
}
