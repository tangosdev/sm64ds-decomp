//cpp
/* dThIcon_c -- the polymorphic touch-screen icon base, ov001.
 *
 * Reconstructed translation unit: the contiguous linker run
 * 0x020ab54c..0x020ab5b0, ROM ordinals 0..1, two functions, assembled from the
 * two one-function legacy sources this change deletes.
 * config/tu_manifest.d/ov001/dThIcon_c.json names both of them.
 *
 * ROM .text, ascending:
 *   0x020ab54c  _ZN9dThIcon_c6RenderEv      ordinal 0  size 0x04
 *   0x020ab550  _ZN9dThIcon_c8BehaviorEv    ordinal 1  size 0x60
 *
 * FUNCTION ORDER HERE IS THE REVERSE OF THE ROM'S, AND THAT IS DELIBERATE.
 * mwccarm 2004/b56 emits one .text section per function, and with codegen
 * deferred -- the default, which this unit leaves alone -- every function body
 * is generated at end of file in the REVERSE of source order. Writing
 * Behavior before Render is therefore what puts Render first in the object, as
 * the cartridge has it. Do not reorder either one to "fix" a diff. No
 * optimisation or codegen pragma is needed anywhere in this unit: both
 * functions reproduce byte-exactly under the build's stock flags.
 *
 * Both of this class's virtuals are defined out of line here and its
 * constructor and destructor are inline in include/dThIcon_c.h, so this TU is
 * where mwccarm anchors the vague-linkage group and emits _ZTV/_ZTI/_ZTS as
 * passengers. All three have ROM homes in ov001 and the manifest's
 * compiler_only_output block licenses exactly those three. Nothing else falls
 * out of this unit: the inline structors are never odr-used here, so no
 * C1/C2/D0/D1/D2 variant is emitted at all and there is no homeless symbol to
 * deadstrip.
 *
 * The ROM's own table, read out of extracted/dsd/arm9_overlays/ov001.bin, is
 * four words at 0x020ad48c: offset-to-top 0, then &_ZTI9dThIcon_c
 * (0x020ad478), then the two dispatch slots -- slot 0 Behavior (0x020ab550),
 * slot 1 Render (0x020ab54c). The _ZTV9dThIcon_c row in
 * config/arm9/overlays/ov001/symbols.txt sits at 0x020ad494, which is the
 * table's ADDRESS POINT, i.e. &_ZTV[2]: it is the value a vptr store writes,
 * not the start of the table. That slot order is what fixes the declaration
 * order in the header, and it is why Behavior is declared first there.
 */

#include "dThIcon_c.h"

extern "C" {

/* The frame delta the per-frame timers across the tree count down by. Spelt
 * `int` here, which is the plurality spelling and the one its scalar users
 * (src/actors/dScMgCup_c.cpp, src/minigames/d_s_mg_base.cpp) already use. */
extern int data_0208ee44;

}

/* -------------------------------------------------------------------------- */
/* ordinal 1 -- 0x020ab550  _ZN9dThIcon_c8BehaviorEv                          */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dThIcon_c8BehaviorEv
/* Vtable slot 0, and the shared tail of the whole hierarchy. Of the three
 * RTTI-proven derived classes, dMgPsOpt_c::TouchIcon_c does not override this
 * slot at all and dispatches straight here; the other two,
 * dScEntry_c::icon_c::Behavior and dScMgSlot1_c::betIcon_c::Behavior, do their
 * own work and then call this explicitly at the tail. Either way every icon in
 * the tree runs this body once a frame.
 *
 * It counts unk_00c down by the global frame delta and does nothing at all
 * until the countdown crosses zero. On the crossing it flips unk_010 between 0
 * and 1, and if unk_014 has gone positive it also latches unk_011 to 1. The
 * leading `<= 0` test is what stops the flip repeating: the timer is left at or
 * below zero after the tick that expires it, so the next frame returns
 * immediately and nothing runs again until something else reloads unk_00c.
 * The countdown and the alternation are ROM-proven; the roles the two flag
 * bytes play for the derived icons are not named by anything in the cartridge
 * and are left as unk_. */
void dThIcon_c::Behavior()
{
    if (unk_00c <= 0)
        return;

    unk_00c -= data_0208ee44;
    if (unk_00c > 0)
        return;

    if (unk_010 != 0)
        unk_010 = 0;
    else
        unk_010 = 1;

    if (unk_014 >= 1)
        unk_011 = 1;
}

/* -------------------------------------------------------------------------- */
/* ordinal 0 -- 0x020ab54c  _ZN9dThIcon_c6RenderEv                            */
/* -------------------------------------------------------------------------- */
// @symbol _ZN9dThIcon_c6RenderEv
/* Vtable slot 1, and empty in the cartridge: a single `bx lr`, four bytes. All
 * three RTTI-proven derived classes replace this entry in their own tables --
 * dMgPsOpt_c::TouchIcon_c, dScEntry_c::icon_c and dScMgSlot1_c::betIcon_c each
 * define a Render -- so this address is reached only through dThIcon_c's own
 * table. The base still has to supply a body rather than leave the slot pure,
 * because the cartridge's table holds this function's address there and a pure
 * slot would put the abort thunk in its place. */
void dThIcon_c::Render()
{
}
