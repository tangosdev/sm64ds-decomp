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

// The func_0203ae58 bridge that used to live here is gone, on the terms its own
// comment set out: it existed because Fader::AdvanceInterp called the 20.12
// approach helper by its historical address-shaped name, which the NDS build
// resolves by address and the host cannot. That extern has now been modernised
// to _Z14ApproachLinearRiii -- the real ROM symbol at 0x0203ae58, defined by
// src/_Z14ApproachLinearRiii.cpp -- which is exactly what a host C++ build emits
// for ApproachLinear(int&, int, int). The name now resolves on both sides
// without help, so bridging it would be a duplicate definition.
