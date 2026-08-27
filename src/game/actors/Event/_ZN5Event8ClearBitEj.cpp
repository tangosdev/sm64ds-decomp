//cpp
// @symbol _ZN5Event8ClearBitEj
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
extern "C" {
extern unsigned int data_0209f34c;
}

namespace Event {

int ClearBit(unsigned int bit)
{
    return data_0209f34c &= ~(1 << bit);
}

}
