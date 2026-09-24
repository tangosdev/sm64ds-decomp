/* RETIRED at run linkfull, lane RS2PORT. This file carried HOST COPIES of the
 * particle subsystem's ARM argument ride-throughs. Every one of them runs as
 * matched code now, so the file defines nothing. It stays an (empty)
 * translation unit only because its enrollment sits inside the three game
 * targets' own source lines in port/CMakeLists.txt, which this lane does not
 * edit; deleting the file and those three entries together is the follow-up.
 *
 * What each copy was for, and why its reason no longer holds:
 *
 * 1. func_0204a5bc, the veneer onto the particle emitter func_0204c584
 *    (`ldr ip, [pc]; bx ip; .word 0x204c584`). src spelled it (void) and let
 *    r0/r1 ride through unnamed. main #3092 (59013376ce) spells both arguments
 *    and forwards them, and retypes its caller func_02049d60 to
 *    (void *manager, void *system); src/func_0204a5bc.c is on
 *    port/slice_w28_rs2port.txt and src/func_02049d60.c (compiled through
 *    hostgen) takes main's text.
 *
 * 2. dPa_c::level_c::simpleCallback_c::SpawnParticles under its flat ROM name.
 *    The copy's reason was that the matched body called func_02049d60 with one
 *    argument and let &sys ride in r1. The matched source passes both,
 *    `func_02049d60(data_0209ee74->mManager, &system)`, and its MSVC member is
 *    what the class's own vtable already calls. The flat name, which
 *    hal/faces4_rows.cpp (the three classes that inherit this body),
 *    hal/particle_bridges.cpp and hal/particle_vtable.cpp still name, is a
 *    reverse face into that member now (port/faces_sync.txt).
 *
 * 3. func_0203cbc0 never lived here (unmatched/func_02073244_hostcopy.c hosted
 *    it); it is retired too, and the matched veneer runs.
 */
