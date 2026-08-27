//cpp
// @symbol _ZN2G212GetBG3ScrPtrEv
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature preserved exactly as it was; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).

namespace G2 {

void* GetBG3ScrPtr()
{
  unsigned int mode = *(volatile unsigned int*)0x4000000 & 7;
  unsigned short bg3cnt = *(volatile unsigned short*)0x400000e;
  unsigned int sbase = ((*(volatile unsigned int*)0x4000000 & 0x38000000) >> 0x1b) << 0x10;
  unsigned int off = ((unsigned int)(bg3cnt & 0x1f00)) >> 8;
  switch(mode){
  case 0: case 1: case 2:
    return (void*)(0x6000000 + sbase + (off << 11));
  case 3: case 4: case 5:
    if(bg3cnt & 0x80) return (void*)(0x6000000 + (off << 0xe));
    return (void*)(0x6000000 + sbase + (off << 11));
  case 6:
    return (void*)0;
  default:
    return (void*)0;
  }
}

}
