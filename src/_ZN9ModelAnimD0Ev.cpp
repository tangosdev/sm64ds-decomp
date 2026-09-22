//cpp
// @symbol _ZN9ModelAnimD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases --
 * which is why both vptr stores appear, the primary over _ZTV9ModelAnim and
 * the Animation subobject's onto VTable_Animation_ModelAnimThunk at +0x50 --
 * then return the object to the heap. Nobody writes that; the same empty
 * `~ModelAnim()` the D1 file defines emits D2, D0 and D1 together, and
 * objisolate keeps the one this file is bound to.
 *
 * The deallocation is Memory::operator_delete2, the family's own inline
 * operator delete -- the same reason the D1 file needs no heap mention.
 */
#include "ModelAnim.h"

ModelAnim::~ModelAnim()
{
}
