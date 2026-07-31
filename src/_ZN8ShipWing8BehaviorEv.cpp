//cpp
// @symbol _ZN8ShipWing8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "ShipWing.h"
void _ZN5Actor9UpdatePosEP12CylinderClsn(void* thiz, void* clsn);
void WithMeshClsn_UpdateContinuous_Veneer(void* p);
int _ZNK12WithMeshClsn10IsOnGroundEv(void* p);
int _ZN5Actor13DistToCPlayerEv(void* p);
void _ZN5Actor14TriplePoofDustEv(void* p);
int _ZN16MeshColliderBase9IsEnabledEv(void* p);
void _ZN16MeshColliderBase7DisableEv(void* p);
void _ZN8Platform21UpdateModelPosAndRotYEv(void* p);
int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* p, int a, int b);
void _ZN8Platform19UpdateClsnPosAndRotEv(void* p);

int ShipWing::Behavior()
{
    switch (mState) {
    case 0:
        if (unk_4e8 == 0) {
            unk_4e9 = 0;
        } else {
            unsigned char* pc9 = (unsigned char*)(((int)((char*)this) + 0x4e9) & 0xFFFFFFFFFFFFFFFF);
            *pc9 = *pc9 + 1;
            unk_4e8 = 0;
        }
        if (unk_4e9 >= 0xf) mState = 1;
        break;
    case 1:
        _ZN5Actor9UpdatePosEP12CylinderClsn(((char*)this), 0);
        WithMeshClsn_UpdateContinuous_Veneer((char*)&mWithMeshClsn);
        if (_ZNK12WithMeshClsn10IsOnGroundEv((char*)&mWithMeshClsn) == 0) {
            if (_ZN5Actor13DistToCPlayerEv(((char*)this)) <= 0x9c4000) break;
        }
        _ZN5Actor14TriplePoofDustEv(((char*)this));
        if (_ZN16MeshColliderBase9IsEnabledEv((char*)&mMeshCollider) != 0) _ZN16MeshColliderBase7DisableEv((char*)&mMeshCollider);
        mPosX = unk_4dc;
        mPosY = unk_4e0;
        mPosZ = unk_4e4;
        mState = 2;
        break;
    case 2: {
        int d = _ZN5Actor13DistToCPlayerEv(((char*)this));
        if (d <= 0x3e8000) break;
        if (d < 0x7d0000) {
            unk_0a8 = 0;
            unk_4e9 = 0;
            unk_4e8 = 0;
            mState = 0;
        }
        break;
    }
    }
    _ZN8Platform21UpdateModelPosAndRotYEv(((char*)this));
    if (mState != 2) {
        if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(((char*)this), 0x5dc000, 0) != 0) {
            _ZN8Platform19UpdateClsnPosAndRotEv(((char*)this));
        }
    }
    return 1;
}
