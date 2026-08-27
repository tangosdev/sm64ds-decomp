//cpp
// @symbol _ZN4cstd14__builtin_trapEv
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
//
// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (compiler trap builtin), so there is no C to decompile it to
// -- the asm block is the faithful source.

namespace cstd {

void __builtin_trap(void)
{
  asm { dcd 0xe7ffffff }
}

}
