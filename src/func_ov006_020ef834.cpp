//cpp
#include "types.h"
// @symbol func_ov006_020ef834
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgJump2_c.h"
// @emits dScMgJump2_c_InitResources
/* recovered: renamed to Class_Method */
/* dScMgJump2_c::InitResources - recovered from vtable slot identity */
extern "C" {
extern s32 GetGameLanguage(void);
extern int LoadFile(int handle);
extern void DecompressLZ16(int src, int dst);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern s32 _ZN4cstd4fdivEii(s32 a, s32 b);
extern void Camera_UpdateMatrices(void *cam);
extern int func_ov006_020c4684(void *ptr, int n);
extern int func_ov006_020c7574(void *base, int count);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *ref);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *bmdFile, int a, int b);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);

extern u8 data_0209d45c;
extern s16 data_02082614;
extern int *data_ov006_021421b8;

typedef struct { int e[12]; } Mtx43T;
extern Mtx43T data_020a0e68;
}

struct Base {
    virtual void m00();
    virtual void m04();
    virtual void m08();
    virtual void m0c();
    virtual void m10();
    virtual void m14();
    virtual void m18();
    virtual void m1c();
    virtual void m20();
    virtual void m24();
    virtual void m28();
    virtual void m2c();
    virtual void m30();
    virtual void m34();
    virtual void m38();
    virtual void m3c();
    virtual void m40();
    virtual void m44();
    virtual void m48(int x);
};

extern "C" int dScMgJump2_c_InitResources(char *base)
{
    struct dScMgJump2_c *self = (struct dScMgJump2_c *)(void *)base;
    int a, b;
    s32 fov;
    void *bmdFile;

    a = LoadFile(data_ov006_0213cce0[GetGameLanguage()]);
    b = LoadFile(0xd1);
    DecompressLZ16(a, 0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)b, 0, 0x80);
    Deallocate((void *)a);
    Deallocate((void *)b);

    data_0209d45c = 0x11;
    _ZN3G3X6SetFogEbiii(0, 0, 2, 0x1000);
    InitialiseVramGlobals();
    *(u16 *)0x4000008 = (*(u16 *)0x4000008 & ~3) | 1;
    fov = _ZN4cstd4fdivEii(0xc0000, (s32)data_02082614);

    self->unk_470c = 0;
    self->unk_4710 = -0x64000;
    self->unk_4714 = 0;
    self->unk_4718 = 0;
    self->unk_471c = 0;
    self->unk_4720 = fov;
    self->unk_4724 = 0x800;
    Camera_UpdateMatrices(base + 0x466c);

    self->unk_47c8 = 0;
    self->unk_47cc = 0x82000;
    self->unk_47d0 = 0;
    self->unk_47d4 = 0;
    self->unk_47d8 = 0;
    self->unk_47dc = fov;
    self->unk_47e0 = 0x800;
    Camera_UpdateMatrices(base + 0x4728);

    if (func_ov006_020c4684(base + 0x5234, 6) == 0)
        return 0;

    func_ov006_020ef0d4((int)(base + 0x57d4), 0x10);

    data_ov006_02140314 = 1;
    if (func_ov006_020c7574(base + 0x500c, 3) == 0)
        return 0;

    data_ov006_021421b8 = &data_ov006_021421c4;
    bmdFile = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov006_021421c4);

    if (_ZN9ModelBase7SetFileEP8BMD_Fileii(base + 0x5a14, bmdFile, 1, -1) == 0) {
        Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
        *(Mtx43T *)(base + 0x5a30) = data_020a0e68;
        return 0;
    }

    func_ov004_020b04d0(0x10);
    ((Base *)base)->m48(-1);
    return 1;
}
