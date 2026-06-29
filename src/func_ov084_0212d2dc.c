typedef int Fix12;
typedef unsigned int u32;

extern u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, Fix12 c, Fix12 d, Fix12 e, const void* f, void* g);
extern void* _ZN8Particle6System12FromUniqueIDEj(u32 id);

extern int data_ov084_02130e24[];

void func_ov084_0212d2dc(char* c)
{
    void* o;
    if (*(int*)(c + 0x170) != data_ov084_02130e24[1])
        return;

    *(u32*)(c + 0x224) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0x224), 0xfc, *(int*)(c + 0x5c), *(int*)(c + 0x60) + 0x1e000, *(int*)(c + 0x64), 0, 0);
    if (*(u32*)(c + 0x224) != 0) {
        o = _ZN8Particle6System12FromUniqueIDEj(*(u32*)(c + 0x224));
        if (o != 0) {
            *(int*)((char*)o + 0x50) = (short)(Fix12)(((long long)(*(int*)(c + 0x210)) * 0x2800 + 0x800) >> 12);
        }
    }

    *(u32*)(c + 0x228) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        *(u32*)(c + 0x228), 0xfd, *(int*)(c + 0x5c), *(int*)(c + 0x60) + 0x1e000, *(int*)(c + 0x64), 0, 0);
    if (*(u32*)(c + 0x228) == 0)
        return;
    o = _ZN8Particle6System12FromUniqueIDEj(*(u32*)(c + 0x228));
    if (o == 0)
        return;
    *(int*)((char*)o + 0x50) = (short)(Fix12)(((long long)(*(int*)(c + 0x210)) * 0x2800 + 0x800) >> 12);
}
