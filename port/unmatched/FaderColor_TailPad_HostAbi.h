/* PORT_HOST_ABI. Give FaderColor the tail the cartridge gives it, so dWipe_c
 * starts its own fields where the ROM starts them. Force-included ahead of the
 * dWipe_c translation units listed in the PORT_TAILPAD_HOSTABI block of
 * port/CMakeLists.txt. Nothing in src/ or include/ changes.
 * Run link100, lane OVERRIDE24.
 *
 * Same defect and same lever as unmatched/dBgActor_c_TailPad_HostAbi.h, which
 * carries the full reading; this is the one class in the set whose base is not
 * dBgActor_c. FaderColor's own field `color` is a u16 at 0x0c, so the class's
 * data ends at 0x0e and its sizeof rounds to 0x10. include/dWipe_c.h says in as
 * many words what it does with the difference:
 *
 *     0x0e/0x0f reuse FaderColor's tail padding -- see notes/scene-provenance.md
 *
 * mwccarm puts unk_00e on 0x0e. MSVC puts it on 0x10, and then every field
 * below it is four bytes high and the object comes out 0x30 against the 0x2c
 * the header records. Capping FaderColor's alignment at two makes its sizeof
 * equal its data size 0x0e, and the whole class falls back onto the cartridge's
 * offsets. FaderColor's own `color` does not move: it is a u16 on 0x0c, which
 * is two-aligned already.
 *
 * FaderColor's size assertion is waived on this side for the same reason
 * dBgActor_c's is, `#ifndef SM64DS_PLATFORM_PC`, so nothing else moves.
 *
 * FaderBrightness.h is pulled in ahead of the pragma so that only FaderColor is
 * compiled under it; it is include/FaderColor.h's whole include list.
 */
#ifndef PORT_FADERCOLOR_TAILPAD_HOSTABI_H
#define PORT_FADERCOLOR_TAILPAD_HOSTABI_H

#include "types.h"
#include "FaderBrightness.h"

#pragma pack(push, 2)
#include "FaderColor.h"
#pragma pack(pop)

#endif
