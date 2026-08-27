//cpp
extern "C" {
extern char* data_ov004_020beb68;
extern int data_ov004_020beb6c;
int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, void*, int, int, int, int, int, int, int, int);
int _ZN3OAM9RenderSubEP7OamAttriiii(void*, int, int, int, int);
typedef int (*GetFn)(void*);
void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4){
  char* g = data_ov004_020beb68;
  if(g == 0) return;
  if(*(int*)(g+0x4628) == 0){
    void** vt = *(void***)g;
    GetFn f = (GetFn)vt[0x1a];
    if(f(g) == 2){
      char* g2 = data_ov004_020beb68;
      if(*(unsigned short*)(g2+0x4664) == 1){
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, a0, a1, a2 + 0xc0 + data_ov004_020beb6c, a3, a4, 0x1000, 0x1000, 0, -1);
        return;
      }
      _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, a0, a1, a2, a3, a4, 0x1000, 0x1000, 0, -1);
      return;
    }
  }
  _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, a0, a1, a2 + 0xc0 + data_ov004_020beb6c, a3, a4, 0x1000, 0x1000, 0, -1);
  _ZN3OAM9RenderSubEP7OamAttriiii(a0, a1, a2, a3, a4);
}
}
