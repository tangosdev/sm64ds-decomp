//cpp
// @symbol _ZN2G212GetBG1ScrPtrEv
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).


namespace G2 {

void* GetBG1ScrPtr(void)
{
  int off = ((*(volatile unsigned short*)0x400000a) & 0x1f00) >> 8;
  unsigned int sbase = (*(volatile unsigned int*)0x4000000) & 0x38000000;
  return (void*)(0x6000000 + ((sbase >> 0x1b) << 0x10) + (off << 11));
}

}
