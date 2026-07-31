//cpp
// @symbol func_ov002_020d6998
/* recovered: shared common types */
#include "common.h"
struct VObj {
    virtual void f00();
    virtual void f01();
    virtual void f02();
    virtual void f03();
    virtual void f04();
    virtual void f05();
    virtual void f06();
    virtual void f07();
    virtual void f08();
    virtual void f09();
    virtual void f10();
    virtual void f11();
    virtual void f12();
    virtual void f13();
    virtual void f14();
    virtual void f15();
    virtual void f16();
    virtual void f17();
    virtual int V18();
};
extern "C" {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef short s16;




typedef int (*VFunc)(void*);

extern int func_ov002_020bea7c(char* c);
extern int _ZN6Player15IsCollectingCapEv(char* self);
extern int func_ov002_020d6dac(char* self);
extern void func_ov002_020d71ec(char* self, int arg1);
extern void* _ZN5Actor10FindWithIDEj(u32 id);
extern int func_ov002_020d6c60(char* self, void* other);
extern s16 Vec3_HorzAngle(const struct Vector3* v0, const struct Vector3* v1);
extern void Vec3_RotateYAndTranslate(int* out, int* in, s16 angle, int* src);
extern void _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, int c, int d, int e, const void* pos, void* cb);
extern int func_ov002_020d7030(char* self, void* other);
extern void Player_DisableInteraction(char* self);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, const struct Vector3* pos);
extern void func_ov002_020db8d8(char* self);
extern void _ZN12CylinderClsn5ClearEv(char* c);
extern void _ZN12CylinderClsn6UpdateEv(char* c);

extern s16 data_02082214[];
extern int func_ov002_020d6998(char* self);
}


int func_ov002_020d6998(char* self)
{
    s16 saved;
    void* other;
    u32 id;

    if (func_ov002_020bea7c(self)) {
        if (_ZN6Player15IsCollectingCapEv(self) != 0) return 0;
        if (*(u8*)(self + 0x6dd) != 3) return 0;
    }
    {
        u16 flag = (u16)(*(u16*)(self + 0x6ce) & 0x1000);
        if (flag) return 0;
    }
    {
        s16* p = (s16*)((((long long)((int)self + 0x8e))));
        s16 diff = *p - *(s16*)(self + 0x764);
        saved = *(s16*)(self + 0x8e);
        *p = diff;
    }
    if (func_ov002_020d6dac(self) != 0) {
        func_ov002_020d71ec(self, 2);
        *(s16*)(self + 0x8e) = saved;
        return 1;
    }
    *(s16*)(self + 0x8e) = saved;

    if (*(u32*)(self + 0x360) != 0) goto fail;
    id = *(u32*)(self + 0x338);
    if (id == 0) goto fail;
    other = _ZN5Actor10FindWithIDEj(id);
    if (other == 0) goto fail;
    if (other == self) goto fail;

    if (_ZN6Player15IsCollectingCapEv(self) != 0) goto react;
    if (((VObj*)other)->V18() == 0) goto react;

    {
        u8 isType = (*(u16*)((char*)other + 0xc) == 0xbf);
        if (isType) {
            if (*(int*)((char*)other + 8) == 3) {
                if (func_ov002_020d6c60(self, other) == 0) goto react;
                return 1;
            } else {
                struct Vector3 out;
                struct Vector3_16 pos16;
                struct Vector3 rotSrc;
                s16 angle;
                int idx;

                angle = Vec3_HorzAngle((struct Vector3*)((char*)other + 0x5c), (struct Vector3*)(self + 0x5c));
                idx = ((u16)angle >> 4) * 2;

                pos16.x = data_02082214[idx];
                pos16.y = 0;
                pos16.z = data_02082214[idx + 1];

                rotSrc.x = 0;
                rotSrc.y = 0x32000;
                rotSrc.z = 0x32000;

                Vec3_RotateYAndTranslate(&out.x, (int*)((char*)other + 0x5c), angle, &rotSrc.x);

                _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(0, 0x13e, out.x, out.y, out.z, &pos16, 0);
                _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(0, 0x13f, out.x, out.y, out.z, &pos16, 0);
                goto react;
            }
        } else {
            int r;
            if (func_ov002_020d7030(self, other) == 0) goto react;
            r = ((VObj*)other)->V18();
            if (r == 4) Player_DisableInteraction(self);
            return 1;
        }
    }

react:
    if (*(u32*)(self + 0x334) & 2) goto fail;
    func_ov002_020d71ec(self, 2);
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(0, 0xfe, (struct Vector3*)(self + 0x74));
    return 1;

fail:
    func_ov002_020db8d8(self);
    _ZN12CylinderClsn5ClearEv(self + 0x314);
    _ZN12CylinderClsn6UpdateEv(self + 0x314);
    return 0;
}
