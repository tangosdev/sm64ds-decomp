#ifndef DTHICON_C_H
#define DTHICON_C_H

#include "types.h"

/* Polymorphic touch-screen icon base. RTTI proves the dThIcon_c name and
 * its derived-class edges. ROM vtables prove the two virtual roles are
 * Behavior and Render; the destructor is nonvirtual. Its 0x24-byte
 * extent is fixed by the derived constructors/destructors and array
 * stride.
 *
 * deslop leftovers:
 * - unk_00c is the countdown this class's TU subtracts data_0208ee44
 *   from; unk_010 flips 0/1 when it expires; unk_011 latches to 1 when
 *   unk_014 >= 1. Original names are not in the cartridge. Naming fans
 *   out to derived Renders.
 * - unk_004 / unk_006 / unk_008 / unk_00a / unk_01c / unk_020 are
 *   unused in this class's TU.
 */
struct dThIcon_c {
    s16 unk_004;
    s16 unk_006;
    s16 unk_008;
    s16 unk_00a;
    s32 unk_00c;
    u8 unk_010;
    u8 unk_011;
    u8 pad_012[0x2];
    s32 unk_014;
    u8 pad_018[0x4];
    s32 unk_01c;
    s32 unk_020;

    dThIcon_c() {}
    ~dThIcon_c() {}
    virtual void Behavior();
    virtual void Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dThIcon_c_size_must_be_0x24[
    sizeof(dThIcon_c) == 0x24 ? 1 : -1];
#endif

#endif
