// @symbol _ZN12CylinderClsnD1Ev
/* recovered: named members + shared header */
#include "CylinderClsn.h"
/* CylinderClsn::~CylinderClsn() at 0x020150a8
 * Complete-object destructor (D1), vtable slot 0. Installs the CylinderClsn
 * vtable, then unlinks this node from the active list. Returns this.
 *
 * func_02014fa4 is NOT a base destructor -- CylinderClsn is a root, its
 * typeinfo is the baseless __class_type_info kind. See include/CylinderClsn.h.
 */

extern int data_0208e6ec[];   // vtable (wildcard reloc, not byte-verified)
extern void func_02014fa4(void* self); // intrusive-list unlink

void* _ZN12CylinderClsnD1Ev(void* self)
{
    *(int*)self = (int)data_0208e6ec; // set vptr
    func_02014fa4(self);
    return self;
}
