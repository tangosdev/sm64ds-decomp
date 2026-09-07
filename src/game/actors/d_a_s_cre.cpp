//cpp
/* Genuine production translation unit for ov018/daSCre_c (4 function(s)),
 * enrolled as one `complete` delinks span.
 *
 * The file stem follows the snake_case scheme in
 * notes/tu-naming-and-swallowers.md sec 1 (tools/tu_names.py):
 * daSCre_c -> d_a_s_cre.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources, all now deleted (ROM
 * address order):
 *   [0] 0x021126d4  src/_ZN8daSCre_cD1Ev.cpp
 *   [1] 0x021126f8  src/_ZN8daSCre_cD0Ev.cpp
 *   [2] 0x02112730  src/_ZN8daSCre_c8BehaviorEv.cpp
 *   [3] 0x0211278c  src/daSCre_c_Spawn.c
 *
 * THE CLASS NAME IS THE CARTRIDGE'S OWN. The decomp used to call this class
 * `PowerStarCreate`, a coined name. MEASURED in
 * extracted/overlays/overlay_0018.bin (ov018 base 0x021111a0, the `.text
 * start:` on line 1 of config/arm9/overlays/ov018/delinks.txt):
 * _ZTS8daSCre_c at 0x02113a38 is the byte string "8daSCre_c", and
 * _ZTI8daSCre_c at 0x02113a44 reads [0x0209a764, 0x02113a38, 0x0208e390] --
 * _ZTVN3abi20__si_class_type_infoE (config/arm9/symbols.txt), that same
 * typeinfo name, and _ZTI8dActor_c, so the ROM states the direct base too.
 * tools/class_rename.py performed the rename, and it is what makes this
 * promotion possible at all: a coined class reaches no symbol home, so its
 * _ZTI/_ZTS/_ZTV records can never be word-compared against the cartridge and
 * its key-function TU cannot declare a verifiable compiler_only_output
 * (tools/tubuild.py apply_compiler_only_policy).
 */

#include "daSCre_c.h"

extern "C" {
extern int _ZTV8daSCre_c[];
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN8dActor_cC2Ev(void *);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- daSCre_c_classInit, 0x0211278c, size 0x30 */
/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves daSCre_c through RTTI,
 * allocation size, vtable identity, and the STAR_CREATE registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: daSCre_c_Spawn. */
extern "C" daSCre_c *daSCre_c_classInit(void)
{
    daSCre_c *p = (daSCre_c *)_ZN7fBase_cnwEj(sizeof(daSCre_c));
    if (p) {
        _ZN8dActor_cC2Ev(p);
        *(int *)p = (int)&_ZTV8daSCre_c[2];
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN8daSCre_c8BehaviorEv, 0x02112730, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN8daSCre_c8BehaviorEv
// recovered name: daSCre_c::Behavior
/* daSCre_c::Behavior - vtable slot 6, overriding fBase_c::Behavior().
 * Migrated to a real member: every field this touches (param1 at 0x008,
 * mPosX/mPosY/mPosZ at 0x05c, mAreaId at 0x0cc) is dActor_c's own, already
 * recovered in include/dActor_c.h -- daSCre_c itself adds no typed
 * fields (include/daSCre_c.h: only pad_0d0[0x4]). dActor_c::Spawn
 * and dActor_c::DistToCPlayer/fBase_c::MarkForDestruction are real
 * (non-virtual) members there. */
s32 daSCre_c::Behavior()
{
    if (DistToCPlayer() < 0x64000) {
        Spawn(0xb2, (param1 & 0xf) | 0x40, *(Vector3 *)&mPosX, 0, mAreaId, -1);
    }
    MarkForDestruction();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN8daSCre_cD1Ev 0x021126d4 size 0x24 and            */
/* _ZN8daSCre_cD0Ev 0x021126f8 size 0x38 -- are NOT written here.               */
/*                                                                             */
/* The destructor is defined INLINE in include/daSCre_c.h. Written out-of-line  */
/* here the real destructor makes mwccarm emit D0 BEFORE D1, the reverse of the */
/* cartridge's order, which objisolate refuses for the whole TU, and it emits a */
/* third D2 body with no ROM home. The inline definition gives the retail D1/D0 */
/* pair in ROM order and no D2, while Behavior -- the first virtual declared    */
/* out-of-line -- keeps this TU as the class's key-function TU, so it still     */
/* owns the complete _ZTV/_ZTI/_ZTS group declared in this entry's              */
/* compiler_only_output. D0 itself is that destructor plus dActor_c's inline    */
/* `operator delete`; see include/dActor_c.h's operator delete note.            */
/* -------------------------------------------------------------------------- */
