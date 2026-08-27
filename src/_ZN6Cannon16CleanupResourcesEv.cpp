//cpp
// @symbol _ZN6Cannon16CleanupResourcesEv
/* Cannon::CleanupResources -- vtable slot 3. Releases the three shared files the class
 * holds; it never touches `this`, which is why the legacy C form could declare
 * itself nullary and still reproduce. */
#include "Cannon.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov098_0213c8e8;
extern SharedFilePtr data_ov098_0213c91c;
extern SharedFilePtr data_ov002_0210da38;

int Cannon::CleanupResources()
{
    data_ov098_0213c8e8.Release();
    data_ov098_0213c91c.Release();
    data_ov002_0210da38.Release();
    return 1;
}
