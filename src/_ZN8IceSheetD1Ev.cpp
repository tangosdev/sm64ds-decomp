//cpp
// @symbol _ZN8IceSheetD1Ev
/* The out-of-line definition makes mwccarm materialize the genuine complete
 * destructor. This per-symbol source also retains the verified vtable/base
 * RTTI contribution that the one-function build measured before migration;
 * objisolate keeps D1 and discards the unlicensed variants and data. */
#include "IceSheet.h"

IceSheet::~IceSheet()
{
}
