/* HOST ABI SHADOW for src/actors/ActorDerived.cpp, and for dBase_c::Spawn
 * alone. Force-included with /FI ahead of that one TU; see the block in
 * port/CMakeLists.txt that names it.
 *
 * WHAT THE CARTRIDGE DOES. dBase_c::Spawn is handed four arguments and names
 * only two of them:
 *
 *     fBase_c *dBase_c::Spawn(u32 actorID, fBase_c *parent, int, int)
 *     { return func_02042ffc(actorID, parent); }
 *
 * and func_02042ffc names the same two. On ARM the third and fourth arguments
 * are already in r2 and r3, so they RIDE THROUGH both functions untouched and
 * arrive in func_02043098 exactly as the caller left them. Both bodies are
 * correct ARM and both are matched, which is why the decomp sources have that
 * shape. port/unmatched/ActorDerived_Spawn.cpp has carried the host-ABI
 * reading and the four-argument copies since gate 14, tagged PORT_HOST_ABI.
 *
 * WHAT MSVC DOES. Arguments are on the stack, so a body that names two
 * forwards two. func_02043098 then reads its third and fourth arguments off
 * the CALLER'S OWN FRAME: measured in the shipped artifact on 2026-09-14,
 * func_02043098 is entered with param1 = 0x001af0ac, which is dBase_c::Spawn's
 * saved ebp, and flags = 0x004bb957, which is dActor_c::Spawn's return
 * address. Every actor spawned through this path is then built with a stack
 * address in fBase_c::mParam at +8, and Player::InitResources unpacks that
 * word: n8 = (0x001af0ac >> 8) & 0xff = 240, func_ov002_020c7dd0 reads
 * data_ov002_020ff240[240] which is 220 bytes past a 20-byte table, and
 * mStateStep becomes 0x70. Player::St_LevelEnter_Init then indexes the
 * 20-word animation table at 0x0210a7e8 with 112 and hands Player::SetAnim a
 * pointer where an animation id belongs.
 *
 * WHY IT CAME BACK. port/CMakeLists.txt's gate-14 filter excludes
 * src/_ZN7dBase_c5SpawnEjP7fBase_cii.cpp so the four-argument host copy owns
 * the symbol. Commit 5116945a1, "Promote ActorDerived as one genuine C++ TU",
 * DELETED that one-function file and folded the function into
 * src/actors/ActorDerived.cpp. The filter now matches nothing, and because
 * ActorDerived.cpp is a real C++ TU its definition carries the MSVC-decorated
 * name ?Spawn@dBase_c@@SAPAUfBase_c@@IPAU2@HH@Z, which is what every C++
 * caller binds to. The host copy kept only the Itanium spelling, which just
 * the C-spelled callers reach. So the argument-dropping body quietly took the
 * call and the ride-through fix stopped applying, with nothing unresolved and
 * no duplicate for the linker to complain about.
 *
 * WHAT THIS HEADER DOES. It renames the static member for this TU only, so
 * ActorDerived.cpp no longer defines the decorated name and the four-argument
 * definition in port/unmatched/ActorDerived_Spawn.cpp can. Nothing else in
 * this TU's include chain uses the token: dBase_c.h line 59 is the sole bare
 * `Spawn` in dBase_c.h, fBase_c.h, types.h and fListNode.h, so the rename
 * reaches the declaration and its one definition and stops there. The renamed
 * body is left compiling and unreferenced rather than deleted, because the
 * decomp source is not this file's to change and the shadow is the record of
 * what the cartridge's own shape is.
 *
 * The two files always travel together: src/actors/ActorDerived.cpp reaches
 * the link through slice_gate10.txt, slice_gate13.txt and slice_scene1.txt,
 * and unmatched/ActorDerived_Spawn.cpp through LEVELBOOT_HOST_SOURCES, and the
 * three executables that take either (smoke_player, walk_window,
 * walk_window_hires) take both.
 *
 * RETIREMENT CONDITION: this header goes away the day dBase_c::Spawn's four
 * arguments are spelled in the decomp source, or the day the port stops
 * compiling src/actors/ActorDerived.cpp.
 */
#ifndef PORT_DBASE_C_SPAWN_HOSTABI_H
#define PORT_DBASE_C_SPAWN_HOSTABI_H

#define Spawn PortHostAbi_dBase_c_Spawn_ArmRideThroughShadow

#endif
