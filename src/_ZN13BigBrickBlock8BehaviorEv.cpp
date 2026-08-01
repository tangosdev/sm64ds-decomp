//cpp
#include "types.h"
// @symbol _ZN13BigBrickBlock8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "BigBrickBlock.h"
extern "C" int _ZN5Event6GetBitEj(unsigned int a);
extern "C" void *_ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void *after);
extern "C" void func_020393a4(int *p, int v);
extern "C" void func_02039394(int *p, int v);
extern "C" void _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void *self, int a, int b);

int BigBrickBlock::Behavior()
{
    int is13 = (int)(mActorId == 0x13);
    if (is13 != 0) {
        if (unk_31f != _ZN5Event6GetBitEj(mEventID))
            unk_31e = 0;

        if (*(void **)((char *)&mSwitch) == 0) {
            unsigned int id = 0xb;
            char *p;
            do {
                *(void **)((char *)&mSwitch) = _ZN5Actor15FindWithActorIDEjPS_(id, *(void **)((char *)&mSwitch));
                p = *(char **)((char *)&mSwitch);
            } while (p == 0 || mEventID != *(u8 *)(p + 0x34e));
        }

        if (_ZN5Event6GetBitEj(mEventID) == 0 || unk_31e != 0) {
            if (_ZN16MeshColliderBase9IsEnabledEv((char *)&mMeshCollider) != 0)
                _ZN16MeshColliderBase7DisableEv((char *)&mMeshCollider);
        } else {
            func_020393a4((int *)((char *)&mMeshCollider), 0x15e000);
            func_02039394((int *)((char *)&mMeshCollider), 0x64000);
            _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(((char *)this), 0x150000, 0);
        }

        unk_31f = _ZN5Event6GetBitEj(mEventID);
    } else {
        int v1 = 0x15e000;
        int v5 = 0x64000;
        int t;
        if ((t = (int)(mActorId == 0x10)) != 0 ||
            (t = (int)(mActorId == 0x11)) != 0 ||
            (t = (int)(mActorId == 0x2e)) != 0) {
            v1 = 0x1c2000;
            v5 = 0x96000;
        }
        func_020393a4((int *)((char *)&mMeshCollider), v1);
        func_02039394((int *)((char *)&mMeshCollider), v5);
        _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(((char *)this), 0x600000, 0);
    }
    return 1;
}
