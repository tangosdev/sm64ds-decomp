// @symbol func_ov075_0211a210
// recovered name: dScEntry_c_CleanupResources
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method */
/* dScEntry_c::CleanupResources - byte-matched vs the ROM at mwccarm 2004/b56, strict relocs
   (tools/match.py, size from ov075 symbols.txt). The old vtable-slot-guess
   marker was wrong: the body reproduces the ROM's bytes. VS wiring lane. */
extern void func_020308b4(signed char levelID);
extern void *data_0209d4a8;
extern void _ZN5Sound21UnsetPlayerVoiceGroupEv(void);
int func_ov075_0211a210(signed char levelID)
{
    func_020308b4(levelID);
    if (data_0209b2e8 != (void *)0) {
        Deallocate(data_0209b2e8);
        data_0209b2e8 = (void *)0;
    }
    data_0209d4a8 = (void *)0;
    _ZN5Sound21UnsetPlayerVoiceGroupEv();
    CleanCommonModelDataArr();
    return 1;
}
