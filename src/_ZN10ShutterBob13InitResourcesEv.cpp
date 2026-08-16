//cpp
// @symbol _ZN10ShutterBob13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
#include "ShutterBob.h"
/* The collider used to be reached through a `u8 mMovingMeshCollider` marker this
   class declared itself. It is Platform's, twice removed, and Platform calls it
   mMeshCollider; the byte at that offset has not moved. The two local shadow
   declarations stay: the ROM's Enable takes an Actor by pointer and declaring the
   real types changes the call site. See notes/mwccarm-codegen.md 6az. They are
   RENAMED, though: this header now reaches the real Actor and the real
   MeshColliderBase through Platform, and redefining either name in the same
   translation unit makes mwccarm 2004/b56 die with an internal compiler error
   rather than a diagnostic. */
class ClsnOwner {};
class ClsnEnable {
public:
    void Enable(ClsnOwner *a);
};

extern "C" {
extern int func_ov002_020bad10(void *c, void **f);
}
extern int data_ov014_021145c4;

int ShutterBob::InitResources()
{
    int r4 = func_ov002_020bad10(((char *)this), (void **)&data_ov014_021145c4);
    ((ClsnEnable *)((char *)&mMeshCollider))->Enable((ClsnOwner *)((char *)this));
    return r4;
}
