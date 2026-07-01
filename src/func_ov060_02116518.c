typedef int Fix12;
typedef unsigned int u32;

extern u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    u32 a, u32 b, Fix12 c, Fix12 d, Fix12 e, const void* f);
extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, Fix12 c, Fix12 d, Fix12 e, const void* f, void* g);
extern void* _ZN8Particle6System12FromUniqueIDEj(u32 id);

void func_ov060_02116518(char* self, u32 kind, int a2, int a3)
{
    void* o;

    if (kind == 0xa2 || kind == 0xa4) {
        *(u32*)(self + 0x37c) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            *(u32*)(self + 0x37c), kind, *(int*)(self + 0x5c), *(int*)(self + 0x60) + a3, *(int*)(self + 0x64), 0);
    } else {
        *(u32*)(self + 0x37c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(u32*)(self + 0x37c), kind, *(int*)(self + 0x5c), *(int*)(self + 0x60) + a3, *(int*)(self + 0x64), 0, 0);
    }

    if (*(u32*)(self + 0x37c) != 0 && a2 != 0) {
        o = _ZN8Particle6System12FromUniqueIDEj(*(u32*)(self + 0x37c));
        if (*(int*)(self + 0x360) >= 0x8000) { *(int*)((char*)o + 0x50) = 0x7fff; } else { *(int*)((char*)o + 0x50) = (short)*(int*)(self + 0x360); }

        if (kind == 0x9a) {
            *(int*)((char*)o + 0x44) = (int)(((long long)(*(int*)(self + 0x360)) * 0x2800 + 0x800) >> 12);
            *(int*)((char*)o + 0x4c) = (short)(Fix12)(((long long)(*(int*)(self + 0x360)) * 0xa66 + 0x800) >> 12);
        }
    }

    *(u32*)(self + 0x380) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(self + 0x380), kind + 1, *(int*)(self + 0x5c), *(int*)(self + 0x60) + a3, *(int*)(self + 0x64), 0, 0);
    if (*(u32*)(self + 0x380) == 0)
        return;
    if (a2 == 0)
        return;

    o = _ZN8Particle6System12FromUniqueIDEj(*(u32*)(self + 0x380));
    if (*(int*)(self + 0x360) >= 0x8000) { *(int*)((char*)o + 0x50) = 0x7fff; } else { *(int*)((char*)o + 0x50) = (short)*(int*)(self + 0x360); }

    if (kind != 0x9a)
        return;

    *(int*)((char*)o + 0x44) = (int)(((long long)(*(int*)(self + 0x360)) * 0x2800 + 0x800) >> 12);
    *(int*)((char*)o + 0x4c) = (short)(Fix12)(((long long)(*(int*)(self + 0x360)) * 0xa66 + 0x800) >> 12);
}
