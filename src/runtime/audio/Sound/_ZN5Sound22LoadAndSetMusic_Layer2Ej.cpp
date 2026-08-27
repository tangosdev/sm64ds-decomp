//cpp
// @symbol _ZN5Sound22LoadAndSetMusic_Layer2Ej
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
extern "C" {
extern int data_0209b4ac;
extern int data_0208e43c;
extern int data_0208e440;
extern int data_0209b4a0[];
extern void func_02011dcc(int* g, int x);
}

// The .c version declared this parameter `int`, contradicting its own mangled
// name: "Ej" is unsigned int. In C mode the symbol was a literal string so the
// disagreement was invisible; in C++ the compiler derives the symbol from the
// types, and `int` would emit ...Ei and fail to resolve the existing caller in
// func_ov062_0211a1f4. The mangled name is the ground truth here -- it is what
// symbols.txt records and what the caller references -- so the parameter is
// unsigned int. The body neither compares nor divides it, so codegen is
// unchanged; the ROM build is the proof.

namespace Sound {

void LoadAndSetMusic_Layer2(unsigned int j)
{
  int a=data_0209b4ac;
  int b=data_0208e43c;
  data_0208e440=j;
  if(a!=b) return;
  func_02011dcc(data_0209b4a0, j);
}

}
