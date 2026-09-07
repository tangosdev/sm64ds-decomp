//cpp
// @symbol _ZN5dBgPiD2Ev
/* recovered: real C++ base-object destructor
 *
 * D2 destroys the dBgPi base-object view: store this class's vptr, then run
 * the dBgPc base destructor. The same `~dBgPi()` definition the D1 and D0
 * files carry -- mwcc emits D2, D0 and D1 from one destructor and
 * config/arm9/delinks.txt binds this file to the D2 variant.
 */
#include "dBgPi.h"

dBgPi::~dBgPi()
{
}
