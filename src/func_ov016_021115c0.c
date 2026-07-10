extern void _ZN7PathPtrC1Ev(void* self);
extern void _ZN7PathPtr6FromIDEj(void* self, unsigned int id);
extern void _ZNK7PathPtr7GetNodeER7Vector3j(void* self, void* out, unsigned int idx);
extern short Vec3_HorzAngle(const void* a, const void* b);
extern int AngleDiff(int a, int b);
extern void _Z14ApproachLinearRsss(short* p, short a, short b);
extern short Vec3_VertAngle(const void* a, const void* b);
extern void Vec3_Sub(void* out, void* a, void* b);
extern int LenVec3(void* v);
extern void func_ov016_02111bf0(void* c, void* p);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalar(void* out, void* v, int s);
extern void SubVec3(void* a, void* b, void* c);
extern void func_02012694(int a, void* p);

extern char data_ov016_02114d7c;

struct Vec3 { int x, y, z; };

#define LA(p) ((void*)(unsigned long)(((long long)(int)(unsigned long)(p)) & 0xFFFFFFFFFFFFFFFFLL))

int func_ov016_021115c0(char* c)
{
    char pp[8];
    struct Vec3 node;
    struct Vec3 diff;
    struct Vec3 scaled;
    int len;
    short ha;

    _ZN7PathPtrC1Ev(pp);
    _ZN7PathPtr6FromIDEj(pp, *(unsigned*)(c + 0x404));
    _ZNK7PathPtr7GetNodeER7Vector3j(pp, &node, *(unsigned*)(c + 0x410));

    ha = Vec3_HorzAngle(c + 0x5c, &node);
    {
        int d = AngleDiff(ha, *(short*)(c + 0x94));
        int s = d + ((unsigned)d >> 31);
        *(short*)((char*)LA(c + 0x400) + 0x26) = (short)(s >> 1);
    }
    _Z14ApproachLinearRsss((short*)(c + 0x94), ha, 0x80);

    _Z14ApproachLinearRsss((short*)(c + 0x92), Vec3_VertAngle(c + 0x5c, &node), 0x80);

    Vec3_Sub(&diff, c + 0x5c, &node);
    len = LenVec3(&diff);
    if (len == 0 || len <= 0xa000) {
        if (*(int*)(c + 0x408) == 1 && *(int*)(c + 0x410) == 0) {
            *(int*)(c + 0x410) = 1;
            func_ov016_02111bf0(c, &data_ov016_02114d7c);
            return 1;
        }
        {
            int* q = (int*)LA(c + 0x410);
            *q = *q + 1;
        }
        if (*(int*)(c + 0x410) >= *(int*)(c + 0x40c))
            *(int*)(c + 0x410) = 0;
        func_02012694(0xfa, c + 0x74);
    } else {
        Vec3_MulScalar(&scaled, &diff, _ZN4cstd4fdivEii(0xa000, len));
        SubVec3(c + 0x5c, &scaled, c + 0x5c);
    }
    return 1;
}
