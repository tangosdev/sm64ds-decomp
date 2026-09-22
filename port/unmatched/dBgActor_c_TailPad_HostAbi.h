/* PORT_HOST_ABI. Give dBgActor_c the tail the cartridge gives it, so the classes
 * that derive from it start their own fields where the ROM starts them.
 * Force-included ahead of the translation units listed in the
 * PORT_TAILPAD_HOSTABI block of port/CMakeLists.txt. Nothing in src/ or
 * include/ changes. Run link100, lane OVERRIDE24.
 *
 * ---- WHAT IS WRONG WITHOUT THIS ----------------------------------------
 *
 * The Itanium ABI that mwccarm implements lets a derived class put its own
 * fields in its base class's TAIL PADDING: it starts them at the base's DATA
 * size. MSVC will not, and starts them at sizeof(base) instead. dBgActor_c has
 * a data size of 0x31e and a sizeof of 0x320, so every class below it starts
 * its own fields two bytes high on the host, and after alignment that becomes
 * two or four bytes on every field the class owns.
 *
 * include/dBgActor_c.h says so itself, in the comment under unk_31d:
 *
 *     The class ends at 0x31e; sizeof rounds to 0x320, and derived classes do
 *     use that tail padding (daObjRc_Guruguru_c::mAngVelY).
 *
 * CRASH8 found this the hard way: QuestionBlock's ModelAnim was CONSTRUCTED by
 * the ROM's own C factory at +0x320 and USED by the C++ translation units at
 * +0x324, so word 0 of the used address was still the zero the allocator left
 * and the first question block the level loader spawned faulted on a null
 * vtable pointer. Twenty-four further classes have the same defect in a form
 * that cannot be fixed by deleting a filler, because their first own field IS
 * the field that lives in the tail padding and there is no filler to delete.
 *
 * ---- WHAT THIS DOES ----------------------------------------------------
 *
 * It caps dBgActor_c's ALIGNMENT at two for the translation units that need it.
 * A class's sizeof is its data size rounded up to its alignment, so capping the
 * alignment at two makes sizeof(dBgActor_c) equal to its data size, 0x31e, and
 * MSVC then starts the derived class's own fields on the cartridge's offset.
 *
 * pack(2) is not packing. It caps alignment; it cannot move a member that is
 * already on a correctly aligned offset, and every member of dBgActor_c already
 * is. MEASURED, not modelled -- MSVC's own /d1reportSingleClassLayout, the
 * whole difference between the two dumps of daObjWanwanShutter_c:
 *
 *     WITHOUT   class daObjWanwanShutter_c size(804)
 *                 <alignment member> (size=2)     <- the base tail MSVC keeps
 *               800 | mDisabled                   <- the ROM says 0x31e
 *                 <alignment member> (size=3)
 *
 *     WITH      class daObjWanwanShutter_c size(800) = 0x320, the ROM's size
 *               798 | mDisabled                   = 0x31e, the ROM's offset
 *                 <alignment member> (size=1)
 *
 * Four lines. Every one of dBgActor_c's own fields, every one of dActor_c's,
 * dBase_c's and fBase_c's, and all thirty-two vtable slots are IDENTICAL in the
 * two dumps. This is the same lever TAILPAD2 used one notch up, where
 * `#pragma pack(push, 4)` under dMgState_c stopped a pointer to member claiming
 * an eight-byte alignment the cartridge never gives it.
 *
 * The size assertion in include/dBgActor_c.h is already waived on this side --
 * it is spelled `#ifndef SM64DS_PLATFORM_PC` -- so nothing else moves. The
 * cartridge never sees this file: it is force-included by the port build only,
 * and mwccarm compiles include/dBgActor_c.h unchanged.
 *
 * ---- WHY THE INCLUDES ABOVE THE PRAGMA ---------------------------------
 *
 * Only dBgActor_c may be packed. Everything include/dBgActor_c.h pulls in is
 * pulled in here FIRST, outside the pragma, so those headers are already
 * guarded by the time the pragma is open and none of them is compiled under it.
 * The list is include/dBgActor_c.h's own include list, in its own order --
 * common.h must stay ahead of Model.h, for the reason that header states.
 */
#ifndef PORT_DBGACTOR_C_TAILPAD_HOSTABI_H
#define PORT_DBGACTOR_C_TAILPAD_HOSTABI_H

#include "types.h"
#include "common.h"
#include "dActor_c.h"
#include "Model.h"
#include "dBgW_KcMbg.h"

#pragma pack(push, 2)
#include "dBgActor_c.h"
#pragma pack(pop)

#endif
