//cpp
// @symbol _ZN5Sound22StopLoadedMusic_Layer3Ev
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
extern "C" {
extern int data_0209b4ac;
extern int data_0208e444;
extern int data_0208e43c;
extern int data_0208e440;
extern int data_0209b4a0[];
extern void func_02011dcc(int* g, int x);
extern void func_0204fa2c(int* g, int z);
}

namespace Sound {

void StopLoadedMusic_Layer3(void)
{
  int a = data_0209b4ac;
  if(a < 0){
    if(data_0208e444 < 0) return;
  }
  data_0208e444 = -1;
  int b = data_0208e43c;
  if(a == b){
    int e440 = data_0208e440;
    if(e440 >= 0){ func_02011dcc(data_0209b4a0, e440); return; }
  }
  if(a < 0 || b < 0)
    func_0204fa2c(data_0209b4a0, 0);
  else
    func_02011dcc(data_0209b4a0, a);
}

}
