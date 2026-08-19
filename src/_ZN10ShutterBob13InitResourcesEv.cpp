//cpp
// @symbol _ZN10ShutterBob13InitResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * The collider used to be reached through a `u8 mMovingMeshCollider` marker this
 * class declared itself. It is dBgActor_c's, twice removed, and dBgActor_c calls it
 * mMeshCollider; the byte at that offset has not moved.
 *
 * THE TWO LOCAL SHADOW CLASSES ARE GONE, and they had to go rather than be
 * renamed. This file used to carry `class dActor_c {};` and
 * `class dBgW { void Enable(dActor_c *); };`. ShutterBob.h now reaches
 * the real dActor_c and the real dBgW through dBgActor_c, and a second
 * definition of either name in the same translation unit makes mwccarm 2004/b56
 * die with an internal compiler error rather than a diagnostic. Renaming them
 * compiles and byte-matches and is still wrong: the call mangles to
 * _ZN10ClsnEnable6EnableEP9ClsnOwner, which nothing defines, so the relocation
 * lands nowhere. The byte gate cannot see that -- it wildcards relocation slots --
 * and tools/check_references.py reported it as newly unresolvable. Using the real
 * declarations mangles to _ZN4dBgW6EnableEP8dActor_c, which resolves.
 */
#include "ShutterBob.h"
#include "dBgW.h"

extern "C" {
extern int func_ov002_020bad10(void *c, void **f);
}
extern int data_ov014_021145c4;

int ShutterBob::InitResources()
{
    int r4 = func_ov002_020bad10(((char *)this), (void **)&data_ov014_021145c4);
    ((dBgW *)((char *)&mMeshCollider))->Enable((dActor_c *)((char *)this));
    return r4;
}
