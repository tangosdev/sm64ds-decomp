//cpp
// @symbol func_ov027_02111eb4
// recovered name: daPgDfdr_c_InitResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* daPgDfdr_c::InitResources - recovered from vtable slot identity */
struct BMD_File;
struct BTP_File;
struct TextureSequence { static void Prepare(BMD_File &model, BTP_File &animFile); };
extern "C" {
extern void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *o, void *f, int a, int b);
extern void *_ZN9Animation8LoadFileER13SharedFilePtr(void *fp);
extern void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *o, void *f, int i, int fx, unsigned j);
extern void func_ov027_02111994(void *c);
extern void *_ZN12MeshCollider8LoadFileER13SharedFilePtr(void *fp);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void *o, void *kcl, void *m, int fx, short s, void *clps);
extern void func_020393d4(void *p, int v);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void *o, void *act, int a, int b, unsigned c, unsigned d);
extern void _ZN13RaycastGroundC1Ev(void *o);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *o, void *v, void *act);
extern int _ZN13RaycastGround10DetectClsnEv(void *o);
extern void _ZN13RaycastGroundD1Ev(void *o);

extern char data_ov027_02113c7c;
extern char data_ov027_02113c94;
extern char data_ov027_02113c6c;
extern void _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);

struct RG { char pad[0x54]; };

int func_ov027_02111eb4(void *cc)
{
    char *c = (char*)cc;
    int i;
    void *f;
    RG rg;
    int v[3];
    char *act;

    f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov027_02113c7c);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(c + 0x320, f, 1, -1);

    for (i = 0; i < 3; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov027_02112ca4[i]);

    _ZN15TextureSequence8LoadFileER13SharedFilePtr(&data_ov027_02113c94);
    TextureSequence::Prepare(**(BMD_File**)(&data_ov027_02113c7c + 4),
                             **(BTP_File**)(&data_ov027_02113c94 + 4));
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(c + 0x384, *(void**)(&data_ov027_02113c94 + 4), 0, 0x1000, 0);

    *(short*)(c + 0x8e) = (short)0xdd30;
    *(short*)(c + 0x94) = *(short*)(c + 0x8e);
    *(int*)(c + 0x5c) = 0x6c4000;
    *(int*)(c + 0x60) = 0xcb2000;
    *(int*)(c + 0x64) = 0x182bb8;
    func_ov027_02111994(c);

    f = _ZN12MeshCollider8LoadFileER13SharedFilePtr(&data_ov027_02113c6c);
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        c + 0x124, f, c + 0x2ec, 0x199, *(short*)(c + 0x8e), &data_ov027_021130e8);
    func_020393d4(c + 0x124, (int)&_ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);

    *(int*)(c + 0x9c) = 0;
    *(int*)(c + 0xa0) = 0;
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(c + 0x398, c, 0x82000, 0xc8000, 0x800004, 0);
    func_ov027_02111d70(c, 1);

    v[0] = *(int*)(c + 0x5c);
    v[1] = *(int*)(c + 0x60);
    v[2] = *(int*)(c + 0x64);
    v[1] = v[1] + 0x14000;
    _ZN13RaycastGroundC1Ev(&rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, v, 0);
    if (_ZN13RaycastGround10DetectClsnEv(&rg))
        *(int*)(c + 0x60) = *(int*)((char*)&rg + 0x44);
    else
        *(int*)(c + 0x60) = v[1];
    *(int*)(c + 0x80) = 0x1000;
    *(int*)(c + 0x84) = 0x1000;
    *(int*)(c + 0x88) = 0x1000;
    _ZN13RaycastGroundD1Ev(&rg);
    return 1;
}
}
