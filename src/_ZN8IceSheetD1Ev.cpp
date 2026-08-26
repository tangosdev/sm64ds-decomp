//cpp
// @symbol _ZN8IceSheetD1Ev
/* Force mwccarm to materialize the inline class-body complete destructor.
 * objisolate retains the enrolled D1 and discards this forcing helper and the
 * compatibility-name vtable/RTTI passengers. */
#include "IceSheet.h"

void IceSheet_EmitDestructor(IceSheet *sheet)
{
    sheet->~IceSheet();
}
