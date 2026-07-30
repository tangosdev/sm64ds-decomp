//cpp
typedef signed short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;

struct Vector3 { int x, y, z; };

extern "C" {
u16 DecIfAbove0_Short(u16* p);
void func_02012790(u32 a);
void* _ZN5Actor10FindWithIDEj(u32 id);
s16 Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
int AngleDiff(int a, int b);
void* _ZN5Actor15FindWithActorIDEjPS_(u32 id, void* prev);
void _ZN5Actor13SpawnSoundObjEj(void* self, u32 a);
void func_02012694(u32 a, void* pos);
void func_ov064_0211a6ec(void* c, int i);
void _ZN6Player5ShockEj(void* p, u32 a);
}

extern "C" void func_ov064_0211a4c4(unsigned char* c)
{
    Vector3 v;
    unsigned char* a;
    int count;
    int total;
    unsigned char* p;
    int b;
    u32 id;

    DecIfAbove0_Short((u16*)(c + 0x170));
    if (*(u16*)(c + 0x170) == 0x58)
        func_02012790(0xe);

    id = *(u32*)(c + 0x15c);
    if (id == 0)
        return;
    if (*(u16*)(c + 0x170) != 0)
        return;

    a = (unsigned char*)_ZN5Actor10FindWithIDEj(id);
    if (a == 0)
        return;

    b = (*(u16*)(a + 0xc) == 0xbf);
    if (b == 0)
        return;

    {
        u32 base = ((u32)a + 0x5c) & 0xFFFFFFFFFFFFFFFFull;
        v.x = *(int*)base;
        v.y = *(int*)(base + 4);
        v.z = *(int*)(base + 8);
    }
    if (AngleDiff(Vec3_HorzAngle((Vector3*)(c + 0x5c), &v), *(s16*)(c + 0x8e)) >= 0x4000)
        return;

    count = 0;
    total = 0;
    p = (unsigned char*)_ZN5Actor15FindWithActorIDEjPS_(0xd, 0);
    while (p != 0) {
        total++;
        if (p != c) {
            int s = *(int*)(p + 0x16c);
            b = 1;
            if (s != 1 && s != 2)
                b = 0;
            if (b)
                count++;
        }
        p = (unsigned char*)_ZN5Actor15FindWithActorIDEjPS_(0xd, p);
    }

    b = *(u8*)(c + 0x172);
    if (count + 1 == b) {
        if (total == b) {
            _ZN5Actor13SpawnSoundObjEj(c, 0);
            *(u8*)(c + 0x173) = 1;
        } else {
            func_02012790(0x26);
        }
        if (*(u8*)(a + 0x706) != 0)
            func_02012694(0x22, c + 0x74);
        else
            func_02012694(0x20, c + 0x74);
        func_ov064_0211a6ec(c, 1);
        return;
    }

    *(u16*)(c + 0x170) = 0x5a;
    if (*(u8*)(a + 0x6f9) != 0)
        _ZN6Player5ShockEj(a, 0);
    else
        _ZN6Player5ShockEj(a, 1);
    for (;;) {
        p = (unsigned char*)_ZN5Actor15FindWithActorIDEjPS_(0xd, p);
        if (p == 0)
            return;
        if (p != c)
            func_ov064_0211a6ec(p, 0);
    }
}
