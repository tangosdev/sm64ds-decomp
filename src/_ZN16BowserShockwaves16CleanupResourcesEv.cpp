//cpp
// @symbol _ZN16BowserShockwaves16CleanupResourcesEv
/* recovered: shared header, real C++ method
 *
 * Three releases. The first, data_ov060_0211b208, is also released by
 * Bowser::CleanupResources -- the shockwave and the fight that spawns it each
 * hold a reference to the same file.
 */
#include "BowserShockwaves.h"
#include "SharedFilePtr.h"

extern char data_ov060_0211b208;
extern char data_ov060_0211b1f8;
extern char data_ov060_0211b200;

int BowserShockwaves::CleanupResources()
{
    ((SharedFilePtr *)(&data_ov060_0211b208))->Release();
    ((SharedFilePtr *)(&data_ov060_0211b1f8))->Release();
    ((SharedFilePtr *)(&data_ov060_0211b200))->Release();
    return 1;
}
