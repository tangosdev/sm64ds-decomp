//cpp
// @symbol _ZN21MegaMushroomCreateTag13InitResourcesEv
/* MegaMushroomCreateTag::InitResources -- vtable slot 0. Arms the collider that
 * makes the mega-mushroom appear, copies the spawn parameter's low byte into
 * unk_109, and pulls the model file in. */
#include "MegaMushroomCreateTag.h"

extern "C" {
/* Spelled out rather than left as `()`: in C++ an empty parameter list means
   "takes nothing", not "unspecified" as it did in the legacy C form. */
int _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *thiz, char *actor, int f1, int f2, unsigned int a, unsigned int b);
int _ZN5Model8LoadFileER13SharedFilePtr(void *sfp);
extern int data_ov002_0210da30[];
}

int MegaMushroomCreateTag::InitResources()
{
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, (char *)this, 0x12c000, 0x32000, 0x800002, 0x20);
    unk_109 = param1;
    _ZN5Model8LoadFileER13SharedFilePtr(data_ov002_0210da30);
    return 1;
}
