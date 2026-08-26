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
KCL_File* _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(SharedFilePtr&);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(void* thiz, KCL_File*, const Matrix4x3&, int fix, short s, CLPS_Block&);
void func_020393c4(int* p, int v);
}
extern SharedFilePtr* data_ov064_0211adc8[];
extern SharedFilePtr data_ov064_0211c800;
extern CLPS_Block data_ov064_0211baac;

int BowserPuzzlePiece::InitResources()
{
    mType = param1 & 0xf;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(&mModel,
        _ZN5Model8LoadFileER13SharedFilePtr(*data_ov064_0211adc8[mType]), 1, -1);
    func_ov064_02119010(((char*)this));
    func_ov064_02118fa4(((char*)this));
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(data_ov064_0211c800),
        mClsnMat, 0x1000, mAngleY, data_ov064_0211baac);
    func_020393c4((int*)((char*)&mMeshCollider), (int)&func_ov064_021192bc);
    mStateInfo = data_ov064_0211c198[mType];
    mStateIndex = 0;
    unk_32c = 0;
    mMoveTimer = 0;
    mHadClsn = 0;
    mFreezeState = 1;
    mState = 0;
    mCanSpawnCoin = 1;
    return 1;
}
