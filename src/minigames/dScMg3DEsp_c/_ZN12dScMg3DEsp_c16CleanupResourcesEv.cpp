//cpp
// @symbol _ZN12dScMg3DEsp_c16CleanupResourcesEv
#include "dScMg3DEsp_c.h"
#include "SharedFilePtr.h"
/* dScMg3DEsp_c::CleanupResources -- vtable slot 3, ov006 0x020e9cec.
 *
 * Attributed by the vtable, not by the `recovered name:` comment, which said
 * dScMg3DEsp_c_CleanupResources and happens to agree here. dScMg3DEsp_c's own
 * vtable is ov006 0x0213c8c4 and its word at slot 3 (0x0213c8d0) relocates to
 * 0x020e9cec -- so this is this class's own override rather than one it
 * inherits from dScMgSingle3DBase_c. The signature is include/fBase_c.h's
 * own slot 3, `virtual s32 CleanupResources()`.
 *
 * IT ALSO RESOLVES TWO PHANTOM REFERENCES. The pre-migration file released
 * `G0` and `G1` -- names that appear in no module's symbols.txt at all, so
 * the file could never link and its delinks entry carries no `complete`
 * marker. config/arm9/overlays/ov006/relocs.txt names the two words the ROM
 * actually loads, from 0x020e9d14 and 0x020e9d18 (the function's own literal
 * pool, inside its declared 0x30):
 *
 *     to:0x02141e9c   data_ov006_02141e9c
 *     to:0x02141e74   data_ov006_02141e74
 *
 * Both are `kind:bss` in that overlay's symbols.txt. They are file handles for
 * the two models this class holds -- see include/dScMg3DEsp_c.h's banner --
 * but they are GLOBAL singletons, not members, which is why the destructor's
 * own Release() calls are on unrelated globals too and why nothing here is
 * reached through `this`.
 *
 * THEY ARE NOT DECLARED THROUGH decl_common.h, which spells both `void *`.
 * The address of the handle is the argument, so the void* spelling would need
 * `&data_...` and would read as one indirection too many; declaring them as
 * the real SharedFilePtr is what lets the calls be `.Release()`. That is also
 * why this file must not include decl_common.h: the two declarations would
 * conflict. And Release() must come from include/SharedFilePtr.h rather than
 * from a bare `extern void _ZN13SharedFilePtr7ReleaseEv(void *)` -- in a //cpp
 * translation unit that spelling mangles a SECOND time, which is the defect
 * that header's own banner records. */

extern "C" {
extern SharedFilePtr data_ov006_02141e9c;
extern SharedFilePtr data_ov006_02141e74;
}

s32 dScMg3DEsp_c::CleanupResources()
{
    data_ov006_02141e9c.Release();
    data_ov006_02141e74.Release();
    return 1;
}
