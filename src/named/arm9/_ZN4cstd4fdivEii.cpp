//cpp
// @symbol _ZN4cstd4fdivEii
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
typedef int s32;

extern "C" {
extern void _ZN4cstd10fdiv_asyncE5Fix12IiE5Fix12IiE(s32 numerator, s32 denominator);
extern s32 _ZN4cstd11fdiv_resultEv(void);
}

namespace cstd {

s32 fdiv(s32 numerator, s32 denominator)
{
    _ZN4cstd10fdiv_asyncE5Fix12IiE5Fix12IiE(numerator, denominator);
    return _ZN4cstd11fdiv_resultEv();
}

}
