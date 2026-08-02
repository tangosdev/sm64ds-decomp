// Host-side definitions for symbols the slice references but whose NDS
// definitions cannot serve an MSVC build.
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

// Fader::AdvanceInterp calls the 20.12 approach helper by its historical
// name func_0203ae58 (extern "C", by-pointer). The function has since been
// identified and renamed to ApproachLinear(int&, int, int) -- the NDS build
// resolves the old name by address, the host cannot. Bridge, do not edit
// src/: the fader TU keeps matching bytes, and when its extern is one day
// modernised this shim dies loudly as a duplicate.
int ApproachLinear(int &ref, int target, int step);
extern "C" void func_0203ae58(int *value, int target, int step)
{
    ApproachLinear(*value, target, step);
}
