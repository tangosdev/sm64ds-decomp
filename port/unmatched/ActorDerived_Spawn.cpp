/* HOST COPIES of src/actors/ActorDerived.cpp and
 * src/func_02042ffc.c -- the actor-spawn entry pair with its arguments
 * spelled out.
 *
 * The ROM's ActorDerived::Spawn is a two-instruction veneer:
 *
 *     ldr ip, [pc] ; bx ip ; .word func_02042ffc
 *
 * and func_02042ffc names only the first two of the four arguments it was
 * handed, leaving param1 and flags riding in r2/r3 across the call into
 * func_02043098. Both are correct ARM and both are matched, which is why the
 * matched sources have those shapes. Under the MSVC cdecl ABI a veneer that
 * declares no parameters forwards nothing: func_02042ffc would read its two
 * arguments off the veneer's own frame, and func_02043098 would take an actor
 * id out of stack garbage and call through data_020a4bb8[garbage].
 *
 * Stage::LoadClsnAndObjects ends in exactly that call (the minimap and HUD
 * actors, ids 0x14f / 0x14e), so the level boot is the first host code that
 * reaches it. The copies below carry all four arguments the whole way; the
 * behaviour -- null parent means no spawn, the SceneNode is parent + 0x14 --
 * is the matched source's, unchanged.
 *
 * 2026-09-14, run link100 lane CRASH4: THE MSVC-DECORATED NAME IS DEFINED HERE
 * TOO NOW, at the bottom of this file. Until today only the Itanium spelling
 * lived here, which is what the port's C-spelled callers reach. Commit
 * 5116945a1 folded the deleted src/_ZN7dBase_c5SpawnEjP7fBase_cii.cpp into
 * src/actors/ActorDerived.cpp, a real C++ TU, so the two-argument decomp body
 * started carrying ?Spawn@dBase_c@@SAPAUfBase_c@@IPAU2@HH@Z and every C++
 * caller bound to that instead of to this file. The gate-14 filter in
 * port/CMakeLists.txt still names the file that no longer exists, so nothing
 * was unresolved and nothing was duplicated: the ride-through fix simply
 * stopped applying, and the level boot spawned every actor with a stack
 * address in fBase_c::mParam. port/unmatched/dBase_c_SpawnHostAbi.h carries
 * the whole measurement and renames the decomp copy out of the way.
 */
#include "dBase_c.h"

extern "C" {

int func_02043098(unsigned id, void *node, unsigned param1, int flags);

/* PORT_HOST_ABI: ARM register ride-through: src names only the first two of
 * the four arguments, leaving param1 and flags in r2/r3 across the call into
 * func_02043098. See the header. */
int func_02042ffc(unsigned id, void *parent, unsigned param1, int flags)
{
    if (parent == 0)
        return 0;
    return func_02043098(id, (char *)parent + 0x14, param1, flags);
}

/* PORT_HOST_ABI: ARM register ride-through: the ROM veneer leaves param1
 * and flags in r2/r3 across a call that names two arguments. See the
 * header. */
void *_ZN7dBase_c5SpawnEjP7fBase_cii(unsigned id, void *parent,
                                             unsigned param1, int flags)
{
    return (void *)(size_t)func_02042ffc(id, parent, param1, flags);
}

}

/* PORT_HOST_ABI: the same ruling under the name C++ callers actually use.
 * LoadEntranceObjects packs the spawn flags and reaches dActor_c::Spawn, which
 * reaches this; on the cartridge the flags and the trailing 2 ride r2/r3
 * through two functions that name neither. Same body as the Itanium spelling
 * above, so the linker's identical-COMDAT folding may well give both one
 * address. */
fBase_c *dBase_c::Spawn(u32 actorID, fBase_c *parent, int param1, int flags)
{
    return (fBase_c *)(void *)(size_t)func_02042ffc(actorID, parent,
                                                    (unsigned)param1, flags);
}
