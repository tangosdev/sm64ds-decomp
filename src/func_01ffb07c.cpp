//cpp
// @symbol func_01ffb07c
/* MeshCollider, 0x01ffb07c (ITCM). Copies a Vector3 into 0x38..0x40 -- three
 * separate word loads and stores, no base register materialised for either side.
 *
 * Those three fields are declared as scalars (unk_38/unk_3c/unk_40) but behave as
 * one Vector3: this copies a Vector3 argument straight in, and SetFile seeds them
 * to (0x1000, 0, 0), the unit X vector. Left as scalars so this change does not
 * alter a struct other files already build against.
 *
 * extern "C": the symbol is unmangled in config/arm9/symbols.txt.
 */
#include "MeshCollider.h"

extern "C" void func_01ffb07c(MeshCollider *self, const Vector3 *v)
{
    self->unk_38 = v->x;
    self->unk_3c = v->y;
    self->unk_40 = v->z;
}
