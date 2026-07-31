// @symbol _ZN12CylinderClsnD2Ev
/* recovered: named members + shared header */
#include "CylinderClsn.h"
/* CylinderClsn::~CylinderClsn() at 0x02015058
 * Base-object destructor (D2). Installs the CylinderClsn vtable, then runs the
 * base subobject destructor (func_02014fa4). Returns this.
 */

extern int _ZTV12CylinderClsn[];   // vtable (wildcard reloc, not byte-verified)
extern void func_02014fa4(void* self); // base subobject destructor

void* _ZN12CylinderClsnD2Ev(void* self)
{
    *(int*)self = (int)_ZTV12CylinderClsn; // set vptr
    func_02014fa4(self);
    return self;
}
