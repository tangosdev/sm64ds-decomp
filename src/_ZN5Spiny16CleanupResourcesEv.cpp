//cpp
// @symbol _ZN5Spiny16CleanupResourcesEv
/* Spiny::CleanupResources -- vtable slot 3. Releases the three shared files the
 * class holds; it never touches `this`. */
#include "Spiny.h"
#include "SharedFilePtr.h"

extern "C" {
extern SharedFilePtr data_ov077_02127b48;
extern SharedFilePtr data_ov077_02127b38;
extern SharedFilePtr data_ov077_02127c14;
}

int Spiny::CleanupResources()
{
    data_ov077_02127b48.Release();
    data_ov077_02127b38.Release();
    data_ov077_02127c14.Release();
    return 1;
}
