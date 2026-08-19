// @symbol _ZN12CylinderClsnD2Ev
/* recovered: named members + shared header */
#include "CylinderClsn.h"
/* CylinderClsn::~CylinderClsn() at 0x02015058
 * Base-object destructor (D2), reached by direct call from derived
 * destructors, never through a vtable. Installs the CylinderClsn vtable,
 * then unlinks this node from the active list. Returns this.
 *
 * func_02014fa4 is NOT a base destructor -- CylinderClsn is a root, its
 * typeinfo is the baseless __class_type_info kind. See include/CylinderClsn.h.
 */

extern int data_0208e6ec[];   // vtable (wildcard reloc, not byte-verified)
extern void func_02014fa4(void* self); // intrusive-list unlink

void* _ZN12CylinderClsnD2Ev(void* self)
{
    *(int*)self = (int)data_0208e6ec; // set vptr
    func_02014fa4(self);
    return self;
}
