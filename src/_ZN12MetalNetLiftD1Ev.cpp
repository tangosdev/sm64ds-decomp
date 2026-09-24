//cpp
// @symbol _ZN12MetalNetLiftD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body.
 * Vtable slot 16: one vtable store, the members in reverse, then ~dBgActor_c. */
#include "MetalNetLift.h"

#ifdef _MSC_VER
/* THE HOST NEEDS THE ROM'S FLAT D1 NAME, AND MSVC ONLY MAKES ONE DESTRUCTOR.
 * It folds the Itanium D1/D0 pair into the single ??1MetalNetLift@@QAE@XZ,
 * which src/_ZN12MetalNetLiftD0Ev.cpp defines out of line for the port
 * (include/MetalNetLift.h declares ~MetalNetLift() with no slot under
 * _MSC_VER), so this file cannot carry the same definition: the host link
 * refuses the pair (LNK2005). It spells out, in terms of that one host symbol,
 * what the variant this file is enrolled for does: the D1 body, with no delete
 * (slot 16's caller frees). The qualified call is direct. The port's slot 16
 * for METAL_NET_LIFT (aml_d1 in hal/actor_classes_ov064_gate178.cpp) calls
 * this name. The destructor stores an MSVC vftable where the cartridge's D1
 * (0x02117978, ov064) stores _ZTV12MetalNetLift;
 * port/tools/dtor_store_guard.py reads this arm (SRC_ARMS) and re-proves from
 * the ROM body on every build that nothing dispatches before the base
 * destructor restores a ROM-shaped table. Nothing here reaches mwccarm: it
 * builds the `#else` arm and emits the ROM bytes it always emitted. */
extern "C" MetalNetLift *_ZN12MetalNetLiftD1Ev(MetalNetLift *thiz)
{
    thiz->MetalNetLift::~MetalNetLift();  /* the D1 body, through the one host symbol */
    return thiz;
}
#else
MetalNetLift::~MetalNetLift()
{
}
#endif
