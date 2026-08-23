//cpp
// @symbol _ZN10ChillBullyD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is vtable slot 17: destroy through ChillBully, daOts_c's four members in
 * reverse declaration order, and dEnemyBase_c, then return the object to the
 * actor heap. The hand-written version spelled that out -- store the vtable
 * twice, call the four member destructors, chain to dEnemyBase_c, call
 * Memory::Deallocate. All of it comes from the same `~ChillBully()` the D1
 * file declares; the deallocation is the inline dEnemyBase_c::operator
 * delete, which is why nothing here mentions the heap.
 *
 * The identical body in both files is not duplication: D1 and D0 are two of
 * the three functions the compiler emits from one destructor, and each file
 * is bound to one of them by config/arm9/overlays/ov027/delinks.txt.
 */
#include "ChillBully.h"

ChillBully::~ChillBully()
{
}
