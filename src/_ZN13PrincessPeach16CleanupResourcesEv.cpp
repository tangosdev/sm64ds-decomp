//cpp
// @symbol _ZN13PrincessPeach16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * One direct release, then a LOOP over a seven-entry table of pointers. The
 * loop is the finding: unlike every sibling in this overlay, which writes its
 * releases out one per line, Peach's animation files are reached through
 * data_ov085_0212f280 and freed by index. Seven is the count the ROM's `blt`
 * tests against.
 */
#include "PrincessPeach.h"
#include "SharedFilePtr.h"

extern char data_ov085_021304f4;
extern SharedFilePtr *data_ov085_0212f280[];

int PrincessPeach::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov085_021304f4))->Release();
    s32 i = 0;
    do {
        data_ov085_0212f280[i]->Release();
        i++;
    } while (i < 7);
    return 1;
}
