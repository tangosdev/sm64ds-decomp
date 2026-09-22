//cpp
// @symbol _ZN4cstd6strlenEPKc
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).

namespace cstd {

unsigned int strlen(const char *s)
{
  int n = -1;
  unsigned char c;
  do {
    c = *s++;
    n = n + 1;
  } while (c != 0);
  return n;
}

}
