//cpp
// @symbol _ZN4cstd4sqrtEy
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).


namespace cstd {

int sqrt(unsigned long long x)
{
  volatile unsigned short *ime = (volatile unsigned short*)0x4000208;
  unsigned short saved = *ime;
  *ime = 0;
  *(volatile unsigned short*)0x40002b0 = 1;
  *(volatile unsigned long long*)0x40002b8 = x << 2;
  *ime;
  *ime = saved;
  while(*(volatile unsigned short*)0x40002b0 & 0x8000);
  return (*(volatile int*)0x40002b4 + 1) >> 1;
}

}
