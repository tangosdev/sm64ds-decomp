//cpp
// @symbol _ZN4cstd7strncpyEPcPKcj
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).

namespace cstd {

char *strncpy(char *dst, const char *src, unsigned int n)
{
  char *d = dst;
  const unsigned char *s = (const unsigned char *)src;
  if (n == 0) return dst;
  do {
    unsigned char c = *s++;
    char *p = d;
    *d++ = c;
    if (*(unsigned char *)p == 0) {
      while (--n) *d++ = 0;
      return dst;
    }
  } while (--n);
  return dst;
}

}
