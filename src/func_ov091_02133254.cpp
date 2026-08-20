//cpp
// @symbol func_ov091_02133254
/* recovered: shared common types */
#include "common.h"
#include "dBgW.h"
#include "TextureSequence.h"
struct SharedFilePtr;
struct BMD_File;
struct KCL_File;

extern "C" BMD_File *_ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, BMD_File *f, int a, int b);
extern "C" void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *self);
extern "C" void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
extern "C" KCL_File *_ZN7dBgW_Kc8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *self, KCL_File *k, void *m, int fix, short s, void *clps);
extern "C" void func_020393d4(int *p, int v);
extern "C" BTP_File *_ZN15TextureSequence8LoadFileER13SharedFilePtr(SharedFilePtr &f);
extern "C" void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, BTP_File *f, int a, int fix, unsigned int u);
extern "C" int _ZN11ShadowModel10InitCuboidEv(void *self);


struct dBgCh_Gnd { char buf[0x44]; int f44; char rest[8]; };
extern "C" void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd *self);
extern "C" void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd *self, Vector3 *v, void *a);
extern "C" int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd *self);
extern "C" void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd *self);

extern void *_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;

extern "C" int func_ov091_02133254(char *self)
{
    dBgCh_Gnd rg;
    Vector3 v;
    BMD_File *bmd;
    KCL_File *kcl;
    void **p320;
    void *p8;
    void *tp;

    p320 = *(void ***)(self + 0x320);
    bmd = _ZN5Model8LoadFileER13SharedFilePtr(*(SharedFilePtr *)*p320);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0xd4, bmd, 1, -1);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv(self);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv(self);

    p320 = *(void ***)(self + 0x320);
    kcl = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*(SharedFilePtr *)*(p320 + 1));
    p8 = *(void **)((char *)p320 + 8);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        self + 0x124, kcl, self + 0x2ec, 0x199, *(short *)(self + 0x8e), p8);
    func_020393d4((int *)(self + 0x124), (int)&_ZN4dBgW16UpdatePosAndAngsERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);
    ((dBgW *)(self + 0x124))->Enable((dActor_c *)(self));

    tp = *(void **)(*(char **)(self + 0x320) + 0xc);
    if (tp != 0)
    {
        _ZN15TextureSequence8LoadFileER13SharedFilePtr(*(SharedFilePtr *)tp);
        p320 = *(void ***)(self + 0x320);
        TextureSequence::Prepare(
            *(BMD_File *)*(void **)((char *)*p320 + 4),
            *(BTP_File *)*(void **)((char *)*(p320 + 3) + 4));
        p320 = *(void ***)(self + 0x320);
        tp = *(void **)((char *)p320 + 0xc);
        _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
            self + 0x324, (BTP_File *)*(void **)((char *)tp + 4), 0x40000000, 0x1000, 0);
    }

    if (!_ZN11ShadowModel10InitCuboidEv(self + 0x338))
        return 0;

    v.x = *(int *)(self + 0x5c);
    v.y = *(int *)(self + 0x60);
    v.z = *(int *)(self + 0x64);
    v.y = v.y + 0x32000;
    _ZN9dBgCh_GndC1Ev(&rg);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&rg, &v, 0);
    *(int *)(self + 0x394) = v.y;
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&rg))
        *(int *)(self + 0x394) = rg.f44;

    *(int *)(self + 0x390) = *(int *)(self + 0x60) + 0x190000;
    *(int *)(self + 0x60) = *(int *)(self + 0x394);
    *(unsigned char *)(self + 0x39e) = 0x28;
    *(int *)(self + 0x9c) = -0x4000;
    *(int *)(self + 0xa0) = -0x3c000;
    *(int *)(self + 0x98) = 0xc000;
    *(unsigned char *)(self + 0x39f) = 0;
    _ZN9dBgCh_GndD1Ev(&rg);
    return 1;
}
