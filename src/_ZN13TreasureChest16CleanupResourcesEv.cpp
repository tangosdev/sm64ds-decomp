//cpp
// @symbol _ZN13TreasureChest16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Four releases, and the split is the finding: two handles live in this
 * overlay and two in ov002, borrowed from the always-resident module. The
 * ROM's order is own-first, borrowed-after, and is reproduced verbatim.
 *
 * `SharedFilePtr::Release` is now spelt as the member call it is, so the
 * compiler emits _ZN13SharedFilePtr7ReleaseEv itself.
 */
#include "TreasureChest.h"
#include "SharedFilePtr.h"

extern char data_ov064_0211c96c;
extern char data_ov064_0211c964;
extern char data_ov002_0210d9a8;
extern char data_ov002_0210da38;

int TreasureChest::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov064_0211c96c))->Release();
    ((SharedFilePtr *)(&data_ov064_0211c964))->Release();
    ((SharedFilePtr *)(&data_ov002_0210d9a8))->Release();
    ((SharedFilePtr *)(&data_ov002_0210da38))->Release();
    return 1;
}
