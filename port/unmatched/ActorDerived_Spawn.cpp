/* HOST COPIES of src/_ZN12ActorDerived5SpawnEjP9ActorBaseii.cpp and
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
 */
#include <cstddef>   /* size_t (the pointer-width cast below) */

extern "C" {

int func_02043098(unsigned id, void *node, unsigned param1, int flags);

int func_02042ffc(unsigned id, void *parent, unsigned param1, int flags)
{
    if (parent == 0)
        return 0;
    return func_02043098(id, (char *)parent + 0x14, param1, flags);
}

void *_ZN12ActorDerived5SpawnEjP9ActorBaseii(unsigned id, void *parent,
                                             unsigned param1, int flags)
{
    return (void *)(size_t)func_02042ffc(id, parent, param1, flags);
}

}
