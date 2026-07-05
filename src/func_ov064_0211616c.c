typedef unsigned int u32;
typedef int Fix12;
typedef struct { short x, y, z; } Vector3_16f;
struct Callback {};
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 id, u32 param, Fix12 x, Fix12 y, Fix12 z, const Vector3_16f* pos, struct Callback* cb);
extern void _ZN9ActorBase18MarkForDestructionEv(void* self);
int func_ov064_0211616c(char* c) {
    if (*(unsigned short*)(c + 0x100) > 0x1e) {
        _ZN9ActorBase18MarkForDestructionEv(c);
        return 1;
    }
    int *p = (int*)((((int)c + 0x60) & 0xFFFFFFFFFFFFFFFFULL));
    *p = *p - 0x5000;
    *(void**)(c + 0x334) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0x334),
        *(u32*)(*(char**)(c + 0x330) + 0x30),
        *(Fix12*)(c + 0x3a8),
        *(Fix12*)(c + 0x3ac),
        *(Fix12*)(c + 0x3b0),
        (const Vector3_16f*)0,
        (struct Callback*)0);
    *(void**)(c + 0x338) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0x338),
        *(u32*)(*(char**)(c + 0x330) + 0x30) + 1,
        *(Fix12*)(c + 0x3a8),
        *(Fix12*)(c + 0x3ac),
        *(Fix12*)(c + 0x3b0),
        (const Vector3_16f*)0,
        (struct Callback*)0);
    return 0;
}
