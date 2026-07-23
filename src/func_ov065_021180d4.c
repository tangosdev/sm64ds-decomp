typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef long long s64;

struct Actor;
struct Vector3 { int x, y, z; };
struct Vector3_16;

extern s16 data_02082214[];

extern struct Actor* _ZN5Actor10FindWithIDEj(u32 id);
extern int _ZN6Player12Unk_020c9e5cEh(struct Actor* self, u8 a);
extern struct Actor* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
    u32 id, u32 param, const struct Vector3* pos,
    const struct Vector3_16* rot, int a, int b);
extern void func_ov001_020ab110(char* p);
extern void func_ov001_020ab228(char* p, char* self, int a, int b, int c);

int func_ov065_021180d4(char* self)
{
    struct Actor* actor;
    u32 id;

    id = *(u32*)(self + 0x164);
    if (id == 0) goto fail;
    if (!(*(u32*)(self + 0x160) & 0x400000)) goto fail;

    actor = _ZN5Actor10FindWithIDEj(id);
    if (actor == 0) goto fail;

    {
        int b = (int)(*(u16*)((char*)actor + 0xc) == 0xbf);
        if (b == 0) goto fail;
    }
    if (*(u8*)((char*)actor + 0x6de) != 0) goto fail;
    if (_ZN6Player12Unk_020c9e5cEh(actor, 5) != 0) goto fail;

    {
        struct Vector3 vec;
        struct Vector3* pv;
        int idx;
        int speed;

        pv = (struct Vector3*)(((long long)(int)((char*)actor + 0x5c)) &
                   0xFFFFFFFFFFFFFFFFLL);
        vec.x = pv->x;
        vec.y = pv->y;
        vec.z = pv->z;

        idx = *(u16*)((char*)actor + 0x8e) >> 4;
        speed = *(int*)((char*)actor + 0x98);

        vec.y += 0x50000;
        vec.x += (int)(((s64)speed *
                           data_02082214[idx * 2] + 0x800) >> 0xc);
        vec.z += (int)(((s64)speed *
                           data_02082214[idx * 2 + 1] + 0x800) >> 0xc);

        if (_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0x10d, 0x1210, &vec,
                (struct Vector3_16*)(self + 0x8c),
                *(s8*)(self + 0xcc), -1) != 0)
        {
            func_ov001_020ab110(self + 0xd4);
            func_ov001_020ab228(self + 0xd4, self, 2, 0, 0);
            *(u16*)(*(char**)(self + 0x174) + 0xe6) = 0x1e;
        }
    }

    return 1;

fail:
    return 0;
}
