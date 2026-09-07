//cpp
// @symbol _ZN5Sound13PlayCharVoiceEjjRK7Vector3
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
//
// Sound::PlayCharVoice(u32 character, u32 voice, const Vector3&). Thin wrapper
// that looks up a per-character voice-base in data_02075250 and hands off to
// Sound::Play on bank 1.
#include "types.h"

extern "C" {
extern unsigned char data_02075250[];
extern unsigned int _ZN5Sound4PlayEjjRK7Vector3(unsigned int a, unsigned int b, const Vector3& v);
}

namespace Sound {

unsigned int PlayCharVoice(unsigned int a, unsigned int b, const Vector3& v)
{
    return _ZN5Sound4PlayEjjRK7Vector3(1, b + data_02075250[a], v);
}

}
