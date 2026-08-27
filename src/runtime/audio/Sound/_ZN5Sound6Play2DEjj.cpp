//cpp
// @symbol _ZN5Sound6Play2DEjj
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature preserved exactly as it was; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
//
// NOTE: this file declares Play2D as returning void; its callers
// (Sound::PlayBank2_2D / PlayBank3_2D) declare it as returning unsigned int.
// Return type is not part of the Itanium mangling, so both link against the
// same symbol -- but one of the two is wrong. Left as-is deliberately: changing
// it here is a codegen change, not a language-mode flip.
extern "C" {
extern void Player_PlaySoundEffect(int x, unsigned int a, unsigned int b);
extern int data_0209b4a4[];
}

namespace Sound {

void Play2D(unsigned int j1, unsigned int j2)
{
    Player_PlaySoundEffect((int)data_0209b4a4, j1, j2);
}

}
