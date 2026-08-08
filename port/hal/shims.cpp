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

// The func_0203ae58 bridge was deleted here on the reasoning that
// _Z14ApproachLinearRiii is "exactly what a host C++ build emits for
// ApproachLinear(int&, int, int)", making a bridge a duplicate definition.
// That is true of an ITANIUM-ABI host compiler. It is not true of MSVC, which
// emits ?ApproachLinear@@YAHAAHHH@Z -- _Z... is the GCC/Clang spelling, not a
// universal one. On top of that, decl_common.h now declares the symbol inside
// its extern "C" guard, so Fader::AdvanceInterp emits a reference to the
// C-decorated __Z14ApproachLinearRiii, which nothing on this host defines.
// smoke.exe failed to link on exactly that.
//
// So the bridge returns, one layer lower than before: define the Itanium-shaped
// name at C linkage and forward to the real C++ definition in
// src/_Z14ApproachLinearRiii.cpp. Under MSVC those are two distinct symbols,
// so this is not a duplicate definition. On a GCC/Clang host it WOULD be --
// guard it by toolchain if the port ever grows a second host compiler.
int ApproachLinear(int &ref, int target, int step);

extern "C" int _Z14ApproachLinearRiii(int &ref, int target, int step)
{
    return ApproachLinear(ref, target, step);
}
