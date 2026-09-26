// Host-side definitions for symbols the slice references but whose NDS
// definitions cannot serve an MSVC build.
#include <stdio.h>
#include <stdlib.h>
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

// Fader::AdvanceInterp calls the 20.12 approach helper by pointer, and the
// name it calls it by has MOVED. It used to be the historical func_0203ae58;
// src/engine/fader/_ZN5Fader13AdvanceInterpEv.cpp now declares
//   extern "C" void _Z14ApproachLinearRiii(Fix12i *, Fix12i, Fix12i);
// which is exactly the modernisation the old comment here predicted ("when its
// extern is one day modernised this shim dies loudly as a duplicate"). It did
// not die loudly, it died quietly: nothing in this binary asks for
// func_0203ae58 any more, so the face went on defining a name with no caller
// while smoke.exe failed to link on the name that does have one. Renamed to
// the spelling the ROM translation unit uses, same body, same forwarding.
//
// The body it forwards to, ApproachLinear(int &, int, int), is
// src/_Z14ApproachLinearRiii.cpp, which this target already compiles: it is
// the very file the flat name is derived from, and it decorates as
// ?ApproachLinear@@YAHAAHHH@Z now that it compiles as C++. Both sides are
// __cdecl free functions, so this is a name hop and nothing else; the caller
// declares the return void and ignores it, which is the same reading
// port/hal/fader_wipes.cpp:118 records for the window targets.
//
// This file is on smoke.exe alone (port/CMakeLists.txt:1204), so the rename
// cannot reach the two other definitions of func_0203ae58's shape:
// hal/fader_wipes.cpp and hal/link21_rows.cpp are on the window targets and
// carry their own rows.
int ApproachLinear(int &ref, int target, int step);
extern "C" void _Z14ApproachLinearRiii(int *value, int target, int step)
{
    ApproachLinear(*value, target, step);
}

// AND THE DELETE SLOT THE HOST COMPILER INSISTS ON. include/Fader.h:85 gives
// Fader a class operator delete that calls the flat extern "C" name
// _ZN6Memory16operator_delete2EPv (declared at Fader.h:69), and MSVC emits a
// scalar deleting destructor for any class with a virtual destructor, so both
// tests/smoke.cpp and this file carry a reference to that name whether or not
// anything ever deletes a Fader. The ROM body, src/_ZN6Memory16operator_delete2EPv.cpp,
// is a veneer onto _ZdlPv at 0x0203cbcc and ends in the game heap.
//
// GATE 1 HAS NO HEAP. Its whole source list is the C math slice, Timer, and
// the Fader interpolation half (port/CMakeLists.txt:1200): no allocator, no
// Memory::Deallocate, no _ZdlPv. The game's targets link the ROM body itself
// (hal/cxxname_bridge.cpp bridges this flat name onto it, run linkfull lane
// ASMCPORT), and it runs _ZdlPv into the game heap; pulling that heap in here
// to satisfy a slot nothing calls would change what gate 1 proves.
//
// So it is a stub, and a LOUD one, in the same spirit as the inert virtual
// slots above: the harness only ever puts a TestFader on the stack, which runs
// the ordinary destructor and never this path, and if that ever stops being
// true the run says so instead of quietly freeing into a heap that is not
// there.
extern "C" void _ZN6Memory16operator_delete2EPv(void *ptr)
{
    fprintf(stderr,
            "smoke: Memory::operator_delete2(%p) reached, but gate 1 links no "
            "heap -- see hal/shims.cpp\n", ptr);
    abort();
}
