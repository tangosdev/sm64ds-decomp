// Host-side definitions for symbols the slice references but whose NDS
// definitions cannot serve an MSVC build.
#include <new>

#include "Fader.h"
#include "FaderBrightness.h"

// The *D0Ev/*D1Ev translation units define Itanium-mangled destructor names
// as C functions -- that satisfies the NDS link, where the filename IS the
// symbol, but MSVC mangles destructors its own way, so the host needs real
// C++ definitions. The ROM dtors only reset vptrs on the way down; there is
// nothing to release, so empty bodies are faithful.
Fader::~Fader() {}
FaderBrightness::~FaderBrightness() {}

// Base-class virtual slots not yet in the slice. Fader.h declares them
// non-pure because the ROM's Fader vtable carries real entries; their src/
// TUs just have not been pulled in yet. Until they are, the host needs
// SOMETHING in the slots for the vtables to link. Defaults chosen to be
// inert and loud-adjacent: value-returning slots report "at neither end".
void Fader::AdvanceFade() {}
int Fader::SetBackwardTime(u32) { return 0; }
int Fader::SetForwardTime(u32) { return 0; }
int Fader::IsAtStart() { return 0; }
int Fader::IsAtEnd() { return 0; }

// FaderBrightness::AdvanceFade is real matched code but writes the 2D-engine
// palette through GX/GXS plus a CP15 cache flush -- that half waits for the
// video seam. The interpolation half is Fader::AdvanceInterp, which IS in
// the slice, so the gate-1 stub advances the fade level and skips only the
// hardware upload.
void FaderBrightness::AdvanceFade() { AdvanceInterp(); }

// Fader::AdvanceInterp deliberately retains the ROM's Itanium spelling as a C
// symbol. MSVC emits its own decoration for the migrated C++ definition, so the
// host needs a calling-convention-preserving forwarder between those spellings.
int ApproachLinear(int &ref, int target, int step);
extern "C" void _Z14ApproachLinearRiii(Fix12i *value, Fix12i target, Fix12i step)
{
    (void)ApproachLinear(*value, target, step);
}

// include/Fader.h:69 -- and sixteen other headers -- spell Memory::operator_delete2
// as an extern "C" Itanium symbol and call it from an inline operator delete, so
// every class in those hierarchies emits a reference to that spelling. On the NDS
// link the mangled string IS the symbol; MSVC decorates it as a cdecl name that
// needs a real definition.
//
// port/hal/ctor_bridge.cpp defines it for the model-family targets by forwarding to
// Memory::Deallocate. Gate 1 links no Memory layer, so forwarding there would only
// trade one unresolved symbol for another. It forwards to the host global instead,
// which is what the ROM function does in any case: a three-word tail call to _ZdlPv,
// the global operator delete (see src/_ZN6Memory16operator_delete2EPv.cpp, which is
// excluded from the slice with the other *D0Ev TUs).
//
// No target links both this file and ctor_bridge.cpp. If one ever does, the duplicate
// is a link error, which is loud -- not a silent divergence between the two routes.
extern "C" void _ZN6Memory16operator_delete2EPv(void *ptr)
{
    ::operator delete(ptr);
}
