//cpp
// @symbol _ZN4cstd3absEi
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).


namespace cstd {

int abs(int x)
{
  return x<0 ? -x : x;
}

}
