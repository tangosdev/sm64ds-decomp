//cpp
#include "daObjCtMecha05_c.h"

/* The historical name TTC_MovingBar_Spawn is project convention, not the cartridge's:
 * the string "TTC_MovingBar" occurs in no ROM binary. What is ROM-proven is
 * the ADDRESS, 0x0211b328, and that the object constructed there is
 * daObjCtMecha05_c -- the store writes this class's vtable. The spelling is
 * assigned in config/arm9/overlays/ov065/symbols.txt; the unmangled, C ABI
 * shape is likewise a project choice the cartridge cannot confirm or deny,
 * because the image carries no symbol names at all. daObjCtMecha05_c_classInit
 * is a lineage-supported reconstruction from the ROM RTTI class and measured
 * factory shape; exact original SM64DS spelling is not preserved.
 *
 * This function is a separate file because tu_map.py puts 0x0211b328 at the
 * start of the NEXT translation unit, not in daObjCtMecha05_c's
 * 0x211ab60..0x211b328 run. Natural `new` here would select the unresolved
 * global allocator, so the body keeps retail's actor allocator and the
 * measured base/member construction sequence explicitly. */
extern "C" {
extern void *_ZN7fBase_cnwEj(u32 size);
extern void _ZN10dBgActor_cC2Ev(dBgActor_c *actor);
extern void _ZN11ShadowModelC1Ev(ShadowModel *shadow);
extern int _ZTV16daObjCtMecha05_c[];

// @symbol daObjCtMecha05_c_classInit
daObjCtMecha05_c *daObjCtMecha05_c_classInit()
{
    daObjCtMecha05_c *actor =
        (daObjCtMecha05_c *)_ZN7fBase_cnwEj(sizeof(daObjCtMecha05_c));
    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        /* This separately compiled factory imports the configured public
         * address point, so retail's store has addend 0. */
        *(int *)actor = (int)_ZTV16daObjCtMecha05_c;
        _ZN11ShadowModelC1Ev(&actor->mShadowModel);
    }
    return actor;
}
}
