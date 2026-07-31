//cpp
// @symbol func_ov006_020ee690
/* recovered: shared common types, renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types, renamed to Class_Method, RTTI class fields named */
#include "dScMgJump_c.h"
// @emits dScMgJump_c_InitResources
/* recovered: shared common types, renamed to Class_Method */
/* dScMgJump_c::InitResources - recovered from vtable slot identity */
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;

extern "C" {
extern s32 func_ov004_020ad674(void);
extern int LoadFile(int handle);
extern void DecompressLZ16(void *src, void *dst);
extern void _ZN2GX11LoadOBJPlttEPKvjj(void const *src, unsigned int offset, unsigned int size);
extern s32 _ZN4cstd4fdivEii(s32 a, s32 b);
extern void func_ov006_020c0134(void *cam);
extern int func_ov006_020c4684(char *ptr, int n);
extern int func_ov006_020c7574(char *base, int count);
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int b, int c);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);

extern u8 data_0209d45c;
extern s16 data_02082614;

struct Mtx43 { int m[12]; };
extern struct Mtx43 data_020a0e68;
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

extern "C" int dScMgJump_c_InitResources(char *base)
{
    struct dScMgJump_c *self = (struct dScMgJump_c *)(void *)base;
    s32 idx;
    int buf1, buf2;
    s32 fov;
    void *modelFile;
    int r;

    idx = func_ov004_020ad674();
    buf1 = LoadFile(data_ov006_0213cbc8[idx]);
    buf2 = LoadFile(0xd1);
    DecompressLZ16((void *)buf1, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj((void *)buf2, 0, 0x80);
    Deallocate((void *)buf1);
    Deallocate((void *)buf2);

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
    func_ov006_020c0134(base + 0x466c);

    self->unk_47c8 = 0;
    self->unk_47cc = 0x82000;
    self->unk_47d0 = 0;
    self->unk_47d4 = 0;
    self->unk_47d8 = 0;
    self->unk_47dc = fov;
    self->unk_47e0 = 0x800;
    func_ov006_020c0134(base + 0x4728);

    if (func_ov006_020c4684(base + 0x5294, 6) == 0)
        return 0;
    if (func_ov006_020c7574(base + 0x506c, 3) == 0)
        return 0;

    if (*(int *)(base + 8) & 1) {
        data_ov006_02142184 = &data_ov006_02142188;
    } else {
        data_ov006_02142184 = &data_ov006_02142190;
    }
    modelFile = _ZN5Model8LoadFileER13SharedFilePtr(data_ov006_02142184);
    r = _ZN9ModelBase7SetFileEP8BMD_Fileii(base + 0x501c, modelFile, 1, -1);
    if (r == 0) {
        Matrix4x3_FromTranslation(&data_020a0e68, 0, 0, 0);
        *(struct Mtx43 *)(base + 0x5038) = data_020a0e68;
        return 0;
    }
    func_ov004_020b04d0(0x10);
    ((Base *)base)->m48(-1);
    return 1;
}
