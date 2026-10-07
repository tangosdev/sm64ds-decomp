// Host-side definitions for symbols the slice references but whose NDS
// definitions cannot serve an MSVC build.
#include "dFader_c.h"
#include "dFdBrightness_c.h"

// The *D0Ev/*D1Ev translation units define Itanium-mangled destructor names
// as C functions -- that satisfies the NDS link, where the filename IS the
// symbol, but MSVC mangles destructors its own way, so the host needs real
// C++ definitions. The ROM dtors only reset vptrs on the way down; there is
// nothing to release, so empty bodies are faithful.
dFader_c::~dFader_c() {}
dFdBrightness_c::~dFdBrightness_c() {}

// Base-class virtual slots not yet in the slice. dFader_c.h declares them
// non-pure because the ROM's dFader_c vtable carries real entries; their src/
// TUs just have not been pulled in yet. Until they are, the host needs
// SOMETHING in the slots for the vtables to link. Defaults chosen to be
// inert and loud-adjacent: value-returning slots report "at neither end".
void dFader_c::AdvanceFade() {}
int dFader_c::SetBackwardTime(u32) { return 0; }
int dFader_c::SetForwardTime(u32) { return 0; }
int dFader_c::IsAtStart() { return 0; }
int dFader_c::IsAtEnd() { return 0; }

// dFdBrightness_c::AdvanceFade is real matched code but writes the 2D-engine
// palette through GX/GXS plus a CP15 cache flush -- that half waits for the
// video seam. The interpolation half is dFader_c::AdvanceInterp, which IS in
// the slice, so the gate-1 stub advances the fade level and skips only the
// hardware upload.
void dFdBrightness_c::AdvanceFade() { AdvanceInterp(); }

// dFader_c::AdvanceInterp deliberately retains the ROM's Itanium spelling as a C
// symbol. MSVC emits its own decoration for the migrated C++ definition, so the
// host needs a calling-convention-preserving forwarder between those spellings.
int ApproachLinear(int &ref, int target, int step);
extern "C" void _Z14ApproachLinearRiii(Fix12i *value, Fix12i target, Fix12i step)
{
    (void)ApproachLinear(*value, target, step);
}

// Memory::operator_delete2 -- referenced from include/dFader_c.h's inline operator
// delete, and so from every dFader_c class here -- is defined in hal/mem_delete2.cpp.
// It lived here first; smoke_roots and smoke_fs then needed the same definition,
// and a per-target copy of a definition that is not target-specific is the thing
// the move avoids.
