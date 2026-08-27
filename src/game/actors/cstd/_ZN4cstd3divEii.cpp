//cpp
// @symbol _ZN4cstd3divEii
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).


namespace cstd {

int div(int a, int b)
{
  *(volatile unsigned short*)0x4000280 = 0;
  *(volatile int*)0x4000290 = a;
  *(volatile unsigned long long*)0x4000298 = (unsigned)b;
  while(*(volatile unsigned short*)0x4000280 & 0x8000);
  return *(volatile int*)0x40002a0;
}

}
