//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef long long s64;

struct Vec3 { s32 x, y, z; };

extern "C" {
    void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, void* cc);
    void* _ZN5Actor10FindWithIDEj(u32 id);
    void _ZN9ActorBase18MarkForDestructionEv(void* self);
    void Vec3_Sub(void* out, void* a, void* b);
    s32 Vec3_HorzLen(void* v);
    s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
    s32 LenVec3(void* v);
    void _Z14ApproachLinearRsss(void* p, s16 target, s16 step);
}

extern short data_02082214[];

extern "C" void func_ov009_02111234(void* self)
{
    u8* c = (u8*)self;
    _ZN5Actor9UpdatePosEP12CylinderClsn(self, 0);
    u8* f = (u8*)_ZN5Actor10FindWithIDEj(*(u32*)(c+0x178));
    if (f == 0 || *(s32*)(f+0x60) > 0xbb8000) {
        _ZN9ActorBase18MarkForDestructionEv(self);
        return;
    }

    Vec3 diff;
    if (*(u8*)(c+0x180) != 0) {
        Vec3 d1;
        Vec3_Sub(&d1, (void*)(c+0x160), (void*)(c+0x5c));
        diff.x = d1.x;
        diff.y = d1.y;
        diff.z = d1.z;
        s32 hl = Vec3_HorzLen(&diff);
        *(s16*)(c+0x16c) = _ZN4cstd5atan2E5Fix12IiES1_(hl, *(s32*)(c+0x60) + 0xfd8f0000);
        *(s16*)(c+0x16e) = _ZN4cstd5atan2E5Fix12IiES1_(diff.z, diff.x);
    } else {
        Vec3 d2;
        Vec3_Sub(&d2, (void*)(f+0x5c), (void*)(c+0x5c));
        diff.x = d2.x;
        diff.y = d2.y;
        diff.z = d2.z;
        s32 hl = Vec3_HorzLen(&diff);
        *(s16*)(c+0x16c) = _ZN4cstd5atan2E5Fix12IiES1_(hl, -diff.y);
        *(s16*)(c+0x16e) = _ZN4cstd5atan2E5Fix12IiES1_(diff.z, diff.x);
        *(s32*)(c+0x174) = LenVec3(&diff) / 25 + 0x14000;
    }

    _Z14ApproachLinearRsss((void*)(c+0x92), *(s16*)(c+0x16c), 0x8c);
    _Z14ApproachLinearRsss((void*)(c+0x94), *(s16*)(c+0x16e), 0x320);

    s32 hi = 0x3000;
    s32 dd = (s16)(*(s16*)(c+0x94) - *(s16*)(c+0x16e));
    if (dd < -hi) dd = -hi;
    else if (dd > hi) dd = hi;
    *(s16*)(c+0x170) = (s16)dd;
    _Z14ApproachLinearRsss((void*)(c+0x90), *(s16*)(c+0x170), 0x258);

    {
        u16 a = *(u16*)(c+0x92);
        s32 t = *(s32*)(c+0x174);
        s16 s = data_02082214[((a>>4)<<1)+1];
        *(s32*)(c+0x98) = (s32)(((s64)t * s + 0x800) >> 12);
    }
    {
        u16 a = *(u16*)(c+0x92);
        s32 t = *(s32*)(c+0x174);
        s16 s = data_02082214[(a>>4)<<1];
        *(s32*)(c+0xa8) = (s32)(((s64)t * s + 0x800) >> 12);
    }
}
