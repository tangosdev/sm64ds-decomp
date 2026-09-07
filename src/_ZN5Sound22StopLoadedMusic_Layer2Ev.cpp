//cpp
// @symbol _ZN5Sound22StopLoadedMusic_Layer2Ev
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
extern "C" {
extern int data_0209b4ac;
extern int data_0208e440;
extern int data_0209b4a0[];
extern void func_0204fa2c(int* g, int z);
extern void func_02011dcc(int* g, int x);
}

namespace Sound {

void StopLoadedMusic_Layer2(void)
{
  int a=data_0209b4ac;
  if(a<0){
    if(data_0208e440<0) return;
  }
  data_0208e440=-1;
  if(a<0){ func_0204fa2c(data_0209b4a0, 0); return; }
  func_02011dcc(data_0209b4a0, a);
}

}
