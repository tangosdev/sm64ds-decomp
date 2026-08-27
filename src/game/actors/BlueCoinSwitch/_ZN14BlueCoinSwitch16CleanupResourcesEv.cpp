//cpp
// @symbol _ZN14BlueCoinSwitch16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method
 *
 * Disables the collider if it is still enabled -- so a second cleanup is
 * harmless -- then releases the model and collision files.
 */
#include "BlueCoinSwitch.h"
#include "decl_common.h"

extern "C" {
extern int BlueCoinSwitch_ClsnFile[];
extern int BlueCoinSwitch_ModelFile[];
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
}

int BlueCoinSwitch::CleanupResources()
{
    if (_ZN4dBgW9IsEnabledEv(&mMeshCollider)) {
        _ZN4dBgW7DisableEv(&mMeshCollider);
    }
    _ZN13SharedFilePtr7ReleaseEv(BlueCoinSwitch_ModelFile);
    _ZN13SharedFilePtr7ReleaseEv(BlueCoinSwitch_ClsnFile);
    return 1;
}
