// @symbol daObjBlockItemTag_c_classInit_BLK_GNSHELL_TAG
/* Measured C++ factory wall: `new BrickBlock` selects the global allocator,
 * while the cartridge calls fBase_c's actor allocator. Keep the verified C ABI
 * construction sequence with the exact class size, base ctor and vtable. */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV10BrickBlock */
/* Reconstructed source-style name: SM64DS proves daObjBlockItemTag_c through RTTI,
 * allocation size, vtable identity, and the BLK_GNSHELL_TAG registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: GreenShellBlockTag_Spawn. */
int *daObjBlockItemTag_c_classInit_BLK_GNSHELL_TAG(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0xdc);
    if (p) { _ZN8dActor_cC2Ev(p); p[0] = (int)_ZTV10BrickBlock; }
    return p;
}
