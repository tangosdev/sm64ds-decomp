//cpp
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct { s32 x, y, z; } Vector3;
typedef s32 Fix12;

extern char data_ov102_0214ea68;
extern char data_ov102_0214ea58;
extern char data_ov102_0214ea78;

extern "C" {
extern void* _ZN5Actor10FindWithIDEj(u32 id);
extern void func_020ada40(void* c, void* v, void* r4, s32 flag);
extern int _ZN6Player7TryGrabER5Actor(void* p, void* actor);
extern void func_ov102_0214d1f8(void* c, void* p);
extern void func_020105cc(void* c, u32 flags);
extern int func_ov102_0214cf4c(char* c, char* p);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void* p, const Vector3* v, u32 a, Fix12 f, u32 b, u32 c, u32 d);
extern int func_ov002_020cc16c(void* c, void* state);
extern int func_ov102_0214cf98(char* c, int p);
}

extern "C" void func_ov102_0214cbec(char* c)
{
    void* r4;
    s32 r2;
    u32 id;
    Vector3 v1, v2;

    id = *(u32*)(c + 0x134);
    if (id == 0)
        return;
    r4 = _ZN5Actor10FindWithIDEj(id);
    if (!r4)
        return;

    {
        int isBf = (int)(*(u16*)((char*)r4 + 0xc) == 0xbf);
        if (!isBf)
            return;
    }
    if (*(u16*)(c + 0x100) != 0)
        return;

    r2 = *(s32*)(c + 0x130);
    if (r2 & 0x10) {
        s16 v[3];
        v[0] = 0x2000;
        v[1] = 0;
        v[2] = 0;
        func_020ada40(c, v, r4, *(s32*)(c + 0x118) >> 1);
        return;
    }

    if (*(u8*)(c + 0x3c5) != 0) {
        if (*(void**)(c + 0x3ac) != (void*)&data_ov102_0214ea68)
            return;
        if (!(r2 & 0x1000))
            return;
        if (_ZN6Player7TryGrabER5Actor(r4, c) == 0)
            return;
        *(void**)(c + 0x3c0) = r4;
        func_ov102_0214d1f8(c, &data_ov102_0214ea58);
        return;
    }

    if (*(u8*)(c + 0x3c4) != 0) {
        s32 flags = r2 & 0x3c0;
        if (flags) {
            func_020105cc(c, flags);
            func_ov102_0214cf4c(c, (char*)r4);
            return;
        }
        if (*(void**)(c + 0x3ac) != (void*)&data_ov102_0214ea78)
            return;
        v1.x = *(s32*)(c + 0x5c);
        v1.y = *(s32*)(c + 0x60);
        v1.z = *(s32*)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, &v1, 1, 0xc000, 1, 0, 1);
        return;
    }

    if (*(void**)(c + 0x3ac) == (void*)&data_ov102_0214ea78) {
        v2.x = *(s32*)(c + 0x5c);
        v2.y = *(s32*)(c + 0x60);
        v2.z = *(s32*)(c + 0x64);
        _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, &v2, 1, 0xc000, 1, 0, 1);
        return;
    }

    if (*(void**)(c + 0x3ac) != (void*)&data_ov102_0214ea68)
        return;
    if (r2 & 0x26fe0)
        return;
    if (func_ov002_020cc16c(r4, c) != 1)
        return;
    func_ov102_0214cf98(c, (int)r4);
}
