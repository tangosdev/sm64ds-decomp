//cpp
// @symbol _ZN17daObjFloatBoard_c16CleanupResourcesEv
#include "daObjFloatBoard_c.h"
#include "SharedFilePtr.h"
/* daObjFloatBoard_c::CleanupResources -- vtable slot 3, ov002 0x020b5be0.
 *
 * Named from the table: 0x020b5be0 is the word slot 3 of
 * _ZTV17daObjFloatBoard_c holds where _ZTV10dBgActor_c holds something else, so
 * it is this class's own override of the virtual fBase_c declares. The
 * header already recorded that address against this slot; only the definition
 * was still sitting under a func_ov002_ name.
 *
 * BOTH CALLEES HAD TO STOP BEING `extern`-ed MANGLED NAMES. The pre-migration
 * file reached dBgW and SharedFilePtr through bare
 * `extern int _ZN4dBgW9IsEnabledEv(void *)` declarations, which
 * only work in a C translation unit: a file compiled as C++ -- which this one
 * now is -- mangles that identifier a SECOND time and emits a reference to
 * _Z28_ZN13SharedFilePtr7ReleaseEvPv, which nothing defines. See the note at
 * the top of include/SharedFilePtr.h. Both classes already declare the
 * members, so the calls are spelled as calls and the compiler produces the
 * ROM's symbols the ordinary way.
 *
 * mFileTable is reloaded between the two Release() calls because the ROM
 * reloads it -- Release() can move it, so the second index must come from a
 * fresh read. */

int daObjFloatBoard_c::CleanupResources()
{
    SharedFilePtr **fp;

    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    fp = (SharedFilePtr **)mFileTable;
    fp[0]->Release();
    fp = (SharedFilePtr **)mFileTable;
    fp[1]->Release();
    return 1;
}
