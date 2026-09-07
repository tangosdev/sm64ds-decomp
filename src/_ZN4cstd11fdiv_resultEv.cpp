//cpp
// @symbol _ZN4cstd11fdiv_resultEv
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).


namespace cstd {

int fdiv_result(void)
{
  while(*(volatile unsigned short*)0x4000280 & 0x8000);
  return (int)((*(volatile long long*)0x40002a0 + 0x80000) >> 20);
}

}
