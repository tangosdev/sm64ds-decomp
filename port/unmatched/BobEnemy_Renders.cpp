/* RETIRED BY GATE SLOT5. The file is kept as an empty translation unit
 * because GATE32_HOST_SOURCES in port/CMakeLists.txt still names it, and that
 * list belongs to gate 32 rather than to this gate.
 *
 * It used to hold seven hand copies of matched Render bodies -- BobOmb,
 * Goomba, BobOmbBuddy, ChainChomp, ChainChompFence, KoopaTheQuick and
 * KoopaFlag -- for the reason its old header gave: the host's _ZTV9ModelAnim
 * had Virtual18 where the ROM has Render, so a body that dispatched slot 5
 * through a local six-virtual shadow reached a method that reads one argument
 * more than the shadow passes.
 *
 * That was never a property of these classes. mwccarm spends TWO vtable
 * entries on `virtual ~X()` -- the Itanium D1 complete-object and D0 deleting
 * pair -- and MSVC folded them into one, so every virtual declared after the
 * destructor sat one slot early on the host through the whole model
 * hierarchy. include/ModelBase.h and its six siblings now spell that pair as
 * two plain virtuals under _MSC_VER; the host numbers the table the ROM's way,
 * and nothing in this tree defines _MSC_VER for mwccarm, so no ROM byte moves.
 *
 * The seven matched sources are back in the link through
 * port/slice_slot5.txt, and the faces that let their fill sites in
 * hal/actor_classes_bob_enemy.cpp reach a __thiscall method are in
 * hal/actor_classes_bob_world.cpp. KingBobOmb::Render was never here: it calls
 * Model::Render by its Itanium C name rather than through a shadow, so
 * src/_ZN10KingBobOmb6RenderEv.cpp always served the host build unchanged.
 */
