//cpp
struct Vector3 { int x, y, z; };

struct C;
typedef void (C::*PMF)();

struct PmfEnt { PMF pmf; };

extern "C" {
extern PmfEnt data_ov060_0211aeb4[];
int Vec3_HorzDist(const Vector3* a, const Vector3* b);
short Vec3_HorzAngle(const Vector3* a, const Vector3* b);
int Actor_GetSubtraction(void* self, short a, short b);
}

struct C {
    char pad[4];
};

extern "C" void func_ov060_02112434(unsigned char* thiz)
{
    Vector3 zero;
    zero.x = 0;
    zero.y = 0;
    zero.z = 0;
    *(int*)(thiz + 0x3f4) = Vec3_HorzDist((Vector3*)(thiz + 0x5c), &zero);
    *(short*)(thiz + 0x408) = Vec3_HorzAngle((Vector3*)(thiz + 0x5c), &zero);

    int s0 = Actor_GetSubtraction(thiz, *(short*)(thiz + 0x8e), *(short*)(thiz + 0x406));
    int s1 = Actor_GetSubtraction(thiz, *(short*)(thiz + 0x8e), *(short*)(thiz + 0x408));

    *(int*)(int)(((long long)(int)(thiz + 0x418)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0xff;
    if (s0 < 0x2000)
        *(int*)(int)(((long long)(int)(thiz + 0x418)) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
    if (s1 < 0x3800)
        *(int*)(int)(((long long)((int)thiz + 0x418)) & 0xFFFFFFFFFFFFFFFFLL) |= 4;
    if (*(int*)(thiz + 0x3f4) < 0x3e8000)
        *(int*)(((int)thiz + 0x418) & 0xFFFFFFFFFFFFFFFF) |= 0x10;
    if (*(int*)(thiz + 0x3ec) < 0x352000)
        *(int*)(int)(((unsigned long long)((unsigned)thiz + 0x418))) |= 8;

    (((C*)thiz)->*data_ov060_0211aeb4[*(int*)(thiz + 0x410)].pmf)();

    if (*(int*)(thiz + 0x40c) == 4) return;

    unsigned char lo = thiz[0x41c];
    unsigned char hi = thiz[0x41d];
    if (hi == lo) return;
    if (hi > lo) {
        int v = lo + 0x14;
        if (v >= 0xff) {
            thiz[0x41c] = 0xff;
            return;
        }
        *(unsigned char*)(int)(((long long)(int)(thiz + 0x41c)) & 0xFFFFFFFFFFFFFFFFLL) =
            *(unsigned char*)(int)(((long long)(int)(thiz + 0x41c)) & 0xFFFFFFFFFFFFFFFFLL) + 0x14;
        return;
    }
    {
        int v = lo - 0x14;
        if (v <= 0) {
            thiz[0x41c] = 0;
        } else {
            *(unsigned char*)(int)(((long long)((int)thiz + 0x41c)) & 0xFFFFFFFFFFFFFFFFLL) =
                *(unsigned char*)(int)(((long long)((int)thiz + 0x41c)) & 0xFFFFFFFFFFFFFFFFLL) - 0x14;
        }
    }
}
