//cpp
// NONMATCHING: r5/fp swap on loop-invariant fix(0x199) vs UpdatePosWithTransform callback (div=5)
#pragma opt_common_subs off
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef struct { s32 x, y, z; } Vector3;
typedef struct { s32 m[12]; } Matrix4x3;
typedef s32 Fix12;
struct SharedFilePtr { u32 data[4]; };
struct BMD_File; struct KCL_File; struct CLPS_Block; struct dBgPi; struct Vector3_16;
extern SharedFilePtr data_ov021_021149a0, data_ov021_021149a8, data_ov021_021149b0, data_ov021_021149b8;
extern Matrix4x3 data_ov021_02113a60, data_ov021_02113a80;
extern "C" {
extern void _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* p);
extern void _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(SharedFilePtr* p);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, BMD_File* f, int a, int b);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, KCL_File* f, const Matrix4x3* m, Fix12 fix, s16 s, CLPS_Block* clps);
extern void func_020393d4(int* p, int v);
extern void func_020393c4(int* p, int v);
extern void _ZN4dBgW6EnableEP8dActor_c(void* thiz, void* actor);
extern void _ZN9dBgCh_GndC1Ev(void* thiz);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(void* thiz, const Vector3* pos, void* actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(void* thiz);
extern void _ZN9dBgCh_GndD1Ev(void* thiz);
extern void func_ov021_02111434(void* p);
extern void func_ov021_0211129c(void* p);
}
extern int _ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
extern int func_ov021_02112128, func_ov021_02111fe4, func_ov021_02111f8c, func_ov021_02111f34, func_ov021_02111edc;
struct dBgCh_Gnd { char buf[0x50]; };
extern "C" int _ZN12WorkElevator13InitResourcesEv(char* sl)
{
    int i; char* p; dBgCh_Gnd ray; Vector3 objPos;
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov021_021149a0);
    _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov021_021149a8);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov021_021149b0);
    _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(&data_ov021_021149b8);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(sl + 0xd4, (BMD_File*)data_ov021_021149a0.data[1], 1, -1);
    i = 0; p = sl + 0x320;
    for (; i < 4; i++) { _ZN9ModelBase7SetFileEP8BMD_Fileii(p, (BMD_File*)data_ov021_021149b0.data[1], 1, -1); p += 0x50; }
    *(char*)(sl + 0xc7a) = -1;
    func_ov021_02111434(sl);
    func_ov021_0211129c(sl);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        sl + 0x124, (KCL_File*)data_ov021_021149a8.data[1], (Matrix4x3*)(sl + 0x2ec),
        0x199, *(s16*)(sl + 0x8e), (CLPS_Block*)&data_ov021_02113a60);
    func_020393d4((int*)(sl + 0x124), (int)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    func_020393c4((int*)(sl + 0x124), (int)&func_ov021_02112128);
    _ZN4dBgW6EnableEP8dActor_c(sl + 0x124, sl);
    {
        int j = 0;
        char* mat = sl + 0x460;
        char* col = sl + 0x520;
        CLPS_Block* clps = (CLPS_Block*)&data_ov021_02113a80;
        Fix12 fix = 0x199;
        int upd = (int)&_ZN4dBgW22UpdatePosWithTransformERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;
        SharedFilePtr* kclFile = &data_ov021_021149b8;
        for (; j < 4; j++) {
            _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                col, (KCL_File*)kclFile->data[1], (Matrix4x3*)mat, fix, *(s16*)(sl + 0x8e), clps);
            func_020393d4((int*)col, upd);
            mat += 0x30; col += 0x1c8;
        }
    }
    func_020393c4((int*)(sl + 0x520), (int)&func_ov021_02111fe4);
    func_020393c4((int*)(sl + 0x6e8), (int)&func_ov021_02111f8c);
    func_020393c4((int*)(sl + 0x8b0), (int)&func_ov021_02111f34);
    func_020393c4((int*)(sl + 0xa78), (int)&func_ov021_02111edc);
    { int k = 0; char* col = sl + 0x520; for (; k < 4; k++) { _ZN4dBgW6EnableEP8dActor_c(col, sl); col += 0x1c8; } }
    *(s32*)(sl + 0xc40) = *(s32*)(sl + 0x5c);
    *(s32*)(sl + 0xc44) = *(s32*)(sl + 0x60);
    *(s32*)(sl + 0xc48) = *(s32*)(sl + 0x64);
    objPos.x = *(s32*)(sl + 0x5c); objPos.y = *(s32*)(sl + 0x60); objPos.z = *(s32*)(sl + 0x64);
    objPos.y = objPos.y - 0x14000;
    _ZN9dBgCh_GndC1Ev(&ray);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&ray, &objPos, (void*)0);
    *(s32*)(sl + 0xc6c) = objPos.y;
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&ray)) *(s32*)(sl + 0xc6c) = *(s32*)((char*)&ray + 0x44);
    _ZN9dBgCh_GndD1Ev(&ray);
    return 1;
}
