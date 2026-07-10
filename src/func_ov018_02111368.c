//cpp
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef long long s64;
typedef int Fix12i;

extern "C" {
int _ZN8Platform20UpdateKillByMegaCharEsss5Fix12IiE(void* self, s16 a, s16 b, s16 c, Fix12i d);
u8 DecIfAbove0_Byte(u8* p);
Fix12i Vec3_HorzDist(const void* a, const void* b);
s16 Vec3_HorzAngle(const void* a, const void* b);
int AngleDiff(int a, int b);
void _Z14ApproachLinearRsss(s16* dst, s16 target, s16 step);
u32 _ZN5Sound8PlayLongEjjjRK7Vector3j(u32 a, u32 b, u32 c, void* pos, u32 d);
void _ZN7PathPtrC1Ev(void* self);
void _ZN7PathPtr6FromIDEj(void* self, u32 id);
void _ZNK7PathPtr7GetNodeER7Vector3j(const void* self, void* out, u32 idx);
void Math_Function_0203b14c(void* p, int a, int b, int c, int d);
void Vec3_Sub(void* out, void* a, void* b);
int LenVec3(void* v);
int _ZN4cstd4fdivEii(int a, int b);
void Vec3_MulScalar(void* out, void* v, int scalar);
void SubVec3(void* a, void* b, void* c);
int _ZNK7PathPtr5LoopsEv(const void* self);
void func_ov018_02111278(char* c);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* self, int a, int b);
void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);
}

extern s16 data_02082214[];

struct Vector3 { int x, y, z; };
struct PathPtrObj { int a, b; };

extern "C" int func_ov018_02111368(char* self)
{
    struct PathPtrObj path;
    struct Vector3 nodeA, nodeB, diff, scaled;
    s16 target;
    char* other;

    if (_ZN8Platform20UpdateKillByMegaCharEsss5Fix12IiE(self, 0x2000, 0, 0, 0) != 0)
        return 1;

    if (DecIfAbove0_Byte((u8*)(self + 0x331)) == 0)
        *(u8*)(self + 0x330) = 1;

    other = *(char**)(self + 0x320);
    if (other != 0) {
        char* otherPos = other + 0x5c;
        Fix12i dist = Vec3_HorzDist(self + 0x5c, otherPos);
        target = (s16)((int)(((s64)dist * data_02082214[((u16)(s16)AngleDiff(Vec3_HorzAngle(self + 0x5c, otherPos), *(s16*)(self + 0x8e)) >> 4) * 2 + 1] + 0x800) >> 12) / 256);
    } else {
        if (*(int*)(self + 0x34c) > 0 && *(int*)(self + 0x33c) == 1) {
            if (Vec3_HorzDist(self + 0x5c, self + 0x324) < 0x10000) {
                *(u8*)(self + 0x330) = 0;
                *(u8*)(self + 0x331) = 0x3c;
            }
        }
        target = 0;
    }
    _Z14ApproachLinearRsss((s16*)(self + 0x8c), target, 0xc0);

    if (*(u8*)(self + 0x330) != 0) {
        int idx2;
        int reached;

        *(u32*)(self + 0x350) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(u32*)(self + 0x350), 3, 0x8c, self + 0x74, 0);

        _ZN7PathPtrC1Ev(&path);
        _ZN7PathPtr6FromIDEj(&path, *(u32*)(self + 0x334));

        {
            int step = *(int*)(self + 0x34c);
            int curIdx = *(int*)(self + 0x33c);
            idx2 = curIdx - step;
            reached = 0;
            if (step > 0) {
                if (idx2 < 0) idx2 = *(int*)(self + 0x338) - 1;
            } else {
                if (idx2 >= *(int*)(self + 0x338)) idx2 = 0;
            }
            _ZNK7PathPtr7GetNodeER7Vector3j(&path, &nodeA, curIdx);
        }
        _ZNK7PathPtr7GetNodeER7Vector3j(&path, &nodeB, idx2);

        Math_Function_0203b14c(self + 0x98, 0xa000, 0x200, 0x2000, 0x200);

        Vec3_Sub(&diff, self + 0x5c, &nodeA);
        {
            int len = LenVec3(&diff);
            if (len == 0 || len <= *(int*)(self + 0x98)) {
                *(int*)(self + 0x5c) = nodeA.x;
                reached = 1;
                *(int*)(self + 0x60) = nodeA.y;
                *(int*)(self + 0x64) = nodeA.z;
            } else {
                int scale = _ZN4cstd4fdivEii(*(int*)(self + 0x98), len);
                Vec3_MulScalar(&scaled, &diff, scale);
                SubVec3(self + 0x5c, &scaled, self + 0x5c);
            }
        }

        if (reached) {
            int* pIdx = (int*)((long long)(int)(self + 0x33c) & 0xFFFFFFFFFFFFFFFFLL);
            *pIdx = *pIdx + *(int*)(self + 0x34c);
            if (*(int*)(self + 0x33c) < 0) {
                if (_ZNK7PathPtr5LoopsEv(&path) != 0) {
                    *(int*)(self + 0x33c) = *(int*)(self + 0x338) - 1;
                } else {
                    *(u8*)(self + 0x331) = 0x3c;
                    *(u8*)(self + 0x330) = 0;
                    *(int*)(self + 0x34c) = 1;
                    int* pIdx2 = (int*)((long long)(unsigned int)(self + 0x33c) & 0xFFFFFFFFFFFFFFFFLL);
                    *pIdx2 = *pIdx2 + (*(int*)(self + 0x34c) * 2);
                }
            }
            if (*(int*)(self + 0x33c) >= *(int*)(self + 0x338)) {
                if (_ZNK7PathPtr5LoopsEv(&path) != 0) {
                    *(int*)(self + 0x33c) = 0;
                } else {
                    *(u8*)(self + 0x331) = 0x3c;
                    *(u8*)(self + 0x330) = 0;
                    *(int*)(self + 0x34c) = -1;
                    int* pIdx3 = (int*)((long long)(int)((unsigned)(int)self + 0x33c) & 0xFFFFFFFFFFFFFFFFLL);
                    *pIdx3 = *pIdx3 + (*(int*)(self + 0x34c) * 2);
                }
            }
        }
    } else {
        *(int*)(self + 0x98) = 0;
    }

    func_ov018_02111278(self);

    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0, 0) != 0) {
        _ZN8Platform19UpdateClsnPosAndRotEv(self);
    }
    *(int*)(self + 0x320) = 0;
    return 1;
}
