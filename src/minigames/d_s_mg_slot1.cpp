//cpp
/* Production translation unit for ov006/dScMgSlot1_c_classInit  (1 function).
 *
 * Enrolled and canonical: this file's licensed text range
 * (.text 0x0210d6b8..0x0210d740) is linked into the ROM build.
 *
 * dScMgSlot1_c_classInit at 0x0210d6b8 (ov006), size 0x88.
 * Matched byte-for-byte with mwccarm 2004/b56.
 * flags: -O4,p -enum int -lang c++ -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
 *
 * THIS IS THE FACTORY ONLY, NOT THE WHOLE CLASS. dScMgSlot1_c's own methods
 * still live as per-function files across 0x0210c374..0x0210d6b8 and are a
 * separate, larger promotion; this entry claims only the one factory the
 * delinker already carved as a complete range. See the manifest entry's
 * boundary_evidence for why that cut is DISCLOSED AS UNCONFIRMED.
 *
 * Only one function, so mwccarm's reverse-source-order .text emission has
 * nothing to order here.
 *
 * Assembled from this legacy source, which is this same file: it lived
 * directly under src/ and moved here.  The manifest's legacy_source row
 * keeps its former full path:
 *   [0] 0x0210d6b8  d_s_mg_slot1.cpp
 */
/* STILL MACHINE-SHAPED (audit 2026-09-18) -- byte-exact; what blocks each part:
 *  2 func_ov006_*                unnamed in config symbols.txt; each needs a
 *                                coined, behaviour-justified name.
 *  2 ctor/dtor/op-new call(s)    C1/C2/D0/D1/D2 is not expressible
 *                                in C++ source; only a real ctor emits it.
 *  3 _ZTV vptr store(s)          stands in for the ctor that would emit it.
 */

#include "dScMgSlot1_c.h"

extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned int sz);
extern void _ZN11dScMgBase_cC2Ev(void *);
extern void func_ov006_0210c2b0(char *p);
extern void func_ov006_0210c208(char *p);
extern void *_ZTV12dScMgSlot1_c;
extern void *_ZTVN12dScMgSlot1_c9betIcon_cE;
extern void *_ZTV9dThIcon_c;
void *dScMgSlot1_c_classInit(void);
/* Reconstructed source-style name: SM64DS proves dScMgSlot1_c through RTTI,
 * allocation size, vtable identity, and the MG_SLOT1 registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov006_0210d6b8. */
void *dScMgSlot1_c_classInit(void)
{
    dScMgSlot1_c *scene =
        (dScMgSlot1_c *)_ZN7fBase_cnwEj(sizeof(dScMgSlot1_c));
    if (scene) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(void **)scene = &_ZTV12dScMgSlot1_c;
        /* Taking &scene->mBetIcon folds two address calculations in mwccarm.
           Preserve the original tree while keeping the pointer typed. */
        dScMgSlot1_c::betIcon_c *icon =
            (dScMgSlot1_c::betIcon_c *)((int)scene + 0x4660);
        *(void **)icon = &_ZTV9dThIcon_c;
        *(void **)icon = &_ZTVN12dScMgSlot1_c9betIcon_cE;
        func_ov006_0210c2b0((char *)scene->pad_4684);
        func_ov006_0210c2b0((char *)scene->pad_4690);
        func_ov006_0210c208((char *)scene->pad_469c);
    }
    return scene;
}
}
