//cpp
// @symbol _ZN6Dorrie13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Dorrie.h"
typedef int Fix12;
typedef short s16;
typedef unsigned char u8;

extern void* _ZN5Model8LoadFileER13SharedFilePtr(void* f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* self, void* f, int a, int b);
extern void* _ZN9Animation8LoadFileER13SharedFilePtr(void* f);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* anim, void* file, int b, int fix, unsigned int e);
extern void* _ZN12MeshCollider8LoadFileER13SharedFilePtr(void* f);
extern void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* self, void* kcl, void* mtx, Fix12 r, short s, void* clps);
extern void func_020393d4(void* p, void* v);
extern void func_020393c4(void* p, void* v);
extern void _ZN16MeshColliderBase6EnableEP5Actor(void* self, void* a);
extern void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void* self, void* a, Fix12 r, Fix12 h, void* p, void* q);
extern void _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(void* self, void* a, Fix12 r, Fix12 h, unsigned int e, unsigned int g);
extern void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void* self, void* a, void* pos, Fix12 r, Fix12 h, unsigned int e, unsigned int g);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, void* pos, void* rot, int e, int f);

extern void _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(void);

extern int data_ov065_0211d720;
extern int data_ov002_0210d9c0;
extern void* data_ov065_0211c080[];
extern void* data_ov065_0211c08c[];

int Dorrie::InitResources()
{
    int i;
    void* f = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov065_0211d720);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xec, f, 1, -1);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9c0);
    for (i = 0; i < 3; i++)
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov065_0211c080[i]);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((char*)this) + 0xec, data_ov065_0211d770[1], 0, 0x1000, 0);
    func_ov065_02118cc4(((char*)this));
    func_ov065_02118838(((char*)this));

    {
        int j;
        char* mtx = ((char*)this) + 0x150;
        char* mmc = ((char*)this) + 0x180;
        for (j = 0; j < 7; j++) {
            void* kcl = _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov065_0211c08c[j]);
            _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
                mmc, kcl, mtx, 0x1000, unk_08e, data_ov065_0211cd68[j]);
            if ((unsigned)j <= 2) {
                func_020393d4(mmc, _ZN16MeshColliderBase16UpdatePosAndAngsERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_);
                if (j == 2)
                    func_020393c4(mmc, func_ov065_021195bc);
                else
                    func_020393c4(mmc, func_ov065_021195d0);
            }
            _ZN16MeshColliderBase6EnableEP5Actor(mmc, ((char*)this));
            mtx += 0x200;
            mmc += 0x200;
        }
    }

    {
        unk_1194 = mPosX;
        unk_1198 = mPosY;
        unk_119c = mPosZ;
        /* u64-mask launder: materialize add r2,sl,#0x5c; ldr/str [r2] */
        *(int*)(((int)((char*)this) + 0x5c)) += 0x7d0000;
    }
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
        ((char*)this) + 0xf50, ((char*)this), 0x1e0000, 0xa0000, 0, 0);
    _ZN18MovingCylinderClsn4InitEP5Actor5Fix12IiES3_jj(
        ((char*)this) + 0x110c, ((char*)this), 0xdc000, 0xfa000, 2, 0x20);
    {
        int v[3];
        v[0] = 0;
        v[1] = 0x50000;
        v[2] = 0x150000;
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
            ((char*)this) + 0x1140, ((char*)this), v, 0x60000, 0x1b0000, 2, 0x400000);
    }
    {
        unk_1180 = mPosX;
        unk_1184 = mPosY;
        unk_1188 = mPosZ;
        unk_11b2 = 0;
        unk_11b5 = 0;
        unk_118c = 0;
        unk_1190 = 0;
        unk_11ac = 0;
        unk_11a8 = unk_11ac;
    }
    if ((mParam & 0xff) == 1)
        unk_0e8 = 1;
    else
        unk_0e8 = 0;
    unk_0d4 = 0;
    if (unk_0e8 != 0) {
        unk_0d8 = 0;
        unk_0dc = 0;
        unk_0e0 = 0;
        func_ov065_02118838(((char*)this));
        {
            void* a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0xa9, 0, ((char*)this) + 0x5c, ((char*)this) + 0x8c, mAreaId, -1);
            if (a != 0) {
                unk_0d4 = (int)a;
                *(int*)((char*)a + 0x174) = (int)((char*)this);
            } else {
                unk_0e8 = 0;
            }
        }
    }
    return 1;
}
