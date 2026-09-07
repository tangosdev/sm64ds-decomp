//cpp
// @symbol _ZN5Sound12PlayBank3_2DEj
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature preserved exactly as it was; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
//
// See the note in _ZN5Sound6Play2DEjj.cpp: the callee's own file declares it
// void. Return type is not mangled, so this declaration is what shapes codegen
// here and is preserved unchanged.

namespace Sound {

unsigned int Play2D(unsigned int, unsigned int);

unsigned int PlayBank3_2D(unsigned int a)
{
    return Play2D(3, a);
}

}
