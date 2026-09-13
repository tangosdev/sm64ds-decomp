/* run link100, lane SEAT5 -- the bridge file for census batch B5 (PMF-SINGLES).
 *
 * Two kinds of line live here and nothing else:
 *
 *  1. THE FLAT C NAME for a seated row whose matched TU is a real C++ member.
 *     src/_ZN5Unagi8BehaviorEv.cpp compiles to ?Behavior@Unagi@@QAEHXZ while the
 *     actor class table calls _ZN5Unagi8BehaviorEv, so the port needs the same
 *     one-line forwarder lane FWD writes in port/hal/fwd_forwarders.cpp for the
 *     twelve PMFB7 Behaviors. Kept in this lane's own file so the fold is
 *     additive.
 *
 *  2. THE C++-LINKAGE DATA SPELLINGS a seated row needs. A //cpp TU whose
 *     `extern ...;` declarations sit at file scope OUTSIDE decl_common.h's
 *     extern "C" block gets each one MANGLED WITH THE TYPE THAT TU CHOSE, while
 *     the overlay mount emits ONE C-named object per symbol. Every LHS below is
 *     a mangled name defined nowhere in the link, read off the linker's own
 *     LNK2019 lines / the TU's own /FAsc EXTRN list
 *     (runs/link100/out/SEAT5/emit_seat5_out.txt), so no alias can be defeated
 *     and alternatename_guard has nothing to refuse. The ov030 / ov032 / ov034 /
 *     ov047 / ov055 recipe.
 */
#include "Unagi.h"

/* src/_ZN5Unagi8BehaviorEv.cpp: `extern void* data_ov016_02114dbc;` at file
   scope -> ?data_ov016_02114dbc@@3PAXA. The cell is ov016 bss; the mount emits
   _data_ov016_02114dbc. */
#pragma comment(linker, "/alternatename:?data_ov016_02114dbc@@3PAXA=_data_ov016_02114dbc")

/* src/func_ov102_021498e0.cpp: `extern PMF data_ov102_0214e870[][4];` and its
   sibling, at file scope -> the array-of-4-pointers-to-member spellings below.
   Both tables are ov102 bss; the mount emits the C names.
   (?data_02082214@@3PAFA and ?data_0209f2f8@@3CA, the other two mangled
   spellings that TU needs, are ALREADY bound in port/hal/cxx_aliases.cpp:744
   and :746 -- measured, not assumed, so they are not repeated here.) */
#pragma comment(linker, "/alternatename:?data_ov102_0214e870@@3PAY03P8C@@AEXXZA=_data_ov102_0214e870")
#pragma comment(linker, "/alternatename:?data_ov102_0214e8c0@@3PAY03P8C@@AEXXZA=_data_ov102_0214e8c0")

extern "C" int _ZN5Unagi8BehaviorEv(void *self)
{
    return ((Unagi *)self)->Unagi::Behavior();
}
