//cpp
// @symbol _ZN17BowserPuzzlePiece13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BowserPuzzlePiece.h"
struct SharedFilePtr { int x; };
struct BMD_File;
struct KCL_File;
struct CLPS_Block { int x; };
extern "C" {
BMD_File* _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr&);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void* thiz, BMD_File*, int, int);
void func_ov064_02119010(char* c);
void func_ov064_02118fa4(void* c);
KCL_File* _ZN12MeshCollider8LoadFileER13SharedFilePtr(SharedFilePtr&);
void _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, KCL_File*, const Matrix4x3&, int fix, short s, CLPS_Block&);
void func_020393c4(int* p, int v);
}
extern SharedFilePtr* data_ov064_0211adc8[];
extern SharedFilePtr data_ov064_0211c800;
extern CLPS_Block data_ov064_0211baac;

void BowserPuzzlePiece::InitResources()
{
    unk_337 = unk_008 & 0xf;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char*)this) + 0xd4,
        _ZN5Model8LoadFileER13SharedFilePtr(*data_ov064_0211adc8[unk_337]), 1, -1);
    func_ov064_02119010(((char*)this));
    func_ov064_02118fa4(((char*)this));
    _ZN18MovingMeshCollider7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        ((char*)this) + 0x124,
        _ZN12MeshCollider8LoadFileER13SharedFilePtr(data_ov064_0211c800),
        *(const Matrix4x3*)((char*)&unk_2ec), 0x1000, unk_08e, data_ov064_0211baac);
    func_020393c4((int*)((char*)&mMovingMeshCollider), (int)&func_ov064_021192bc);
    unk_324 = data_ov064_0211c198[unk_337];
    unk_328 = 0;
    unk_32c = 0;
    unk_334 = 0;
    unk_338 = 0;
    unk_339 = 1;
    unk_336 = 0;
    unk_33a = 1;
}
