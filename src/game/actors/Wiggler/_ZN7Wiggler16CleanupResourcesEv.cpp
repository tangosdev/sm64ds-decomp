//cpp
// @symbol _ZN7Wiggler16CleanupResourcesEv

/* _ZN7Wiggler16CleanupResourcesEv at 0x02112a5c
 *
 * Releases all five entries of each of the seven SharedFilePtr tables this
 * actor pulled, then returns 1.
 */

#include "SharedFilePtr.h"
#include "Wiggler.h"

extern "C" {
extern SharedFilePtr *data_ov034_02113838[];
extern SharedFilePtr *data_ov034_0211384c[];
extern SharedFilePtr *data_ov034_02113860[];
extern SharedFilePtr *data_ov034_02113874[];
extern SharedFilePtr *data_ov034_02113888[];
extern SharedFilePtr *data_ov034_0211389c[];
extern SharedFilePtr *data_ov034_021138b0[];
}

int Wiggler::CleanupResources()
{
    int i;
    for (i = 0; i < 5; i++) {
        data_ov034_02113838[i]->Release();
        data_ov034_0211384c[i]->Release();
        data_ov034_02113860[i]->Release();
        data_ov034_02113874[i]->Release();
        data_ov034_02113888[i]->Release();
        data_ov034_0211389c[i]->Release();
        data_ov034_021138b0[i]->Release();
    }
    return 1;
}
