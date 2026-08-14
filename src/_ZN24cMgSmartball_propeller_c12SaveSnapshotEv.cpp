//cpp
/* Slot 0. Opens with the base's own SaveSnapshot body written out inline
 * rather than called -- same idiom as every sibling. If the base's flag at
 * (char*)unk_004 + 0x595d is set, it returns before touching anything else.
 * Otherwise it steps the rotation speed (unk_036) by +-8 toward the target
 * (unk_034), clamping once it crosses, adds the stepped speed onto the
 * base's own angle at 0x32 (raw cast -- see cMgSmartball_propeller_c.h on
 * why that field isn't reached by name), and calls the shared helper that
 * recomputes the propeller-blade positions from the new angle.
 *
 * KEPT AS RAW OFFSET ACCESS ON PURPOSE, unlike Update/RestoreInitial in
 * this same class. Rewritten with named `unk_034`/`unk_036` member access
 * this compiled to 0x98 bytes against a required 0xac: mwcc CSEs the
 * repeated `this->unk_036` load into fewer reloads than the ROM's shape
 * (write through a locally-cast `short* p`, but re-read every comparison
 * through a fresh `*(short*)(c+0x36)` cast) allows. The local `p`/`q`
 * pointers and repeated casts are load-bearing for the byte match, not
 * decoration -- see cMgSmartball_propeller_c.h. */
#include "cMgSmartball_propeller_c.h"

extern "C" void func_ov006_02110e28(char* c);

void cMgSmartball_propeller_c::SaveSnapshot()
{
    char* c = (char*)this;
    *(int*)(c+0x10) = *(int*)(c+8);
    *(int*)(c+0x14) = *(int*)(c+0xc);
    if (*(unsigned char*)(*(char**)(c+4) + 0x595d) != 0)
        return;
    if (*(short*)(c+0x36) < *(short*)(c+0x34)) {
        short* p = (short*)(((int)c + 0x36));
        *p = *p + 8;
        if (*(short*)(c+0x36) > *(short*)(c+0x34))
            *(short*)(c+0x36) = *(short*)(c+0x34);
    } else {
        short* p = (short*)(((int)c + 0x36));
        *p = *p - 8;
        if (*(short*)(c+0x36) < *(short*)(c+0x34))
            *(short*)(c+0x36) = *(short*)(c+0x34);
    }
    {
        short* q = (short*)(((int)c + 0x32));
        *q = *q + *(short*)(c+0x36);
    }
    func_ov006_02110e28(c);
}
