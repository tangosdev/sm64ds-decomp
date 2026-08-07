//cpp
// @symbol _ZN4cstd4ldivEii
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
extern "C" {
extern void _ZN4cstd10fdiv_asyncE5Fix12IiE5Fix12IiE(int a, int b);
}

namespace cstd {

long long ldiv(int a, int b)
{
  _ZN4cstd10fdiv_asyncE5Fix12IiE5Fix12IiE(a, b);
  while(*(volatile unsigned short*)0x4000280 & 0x8000);
  return *(volatile long long*)0x40002a0;
}

}
