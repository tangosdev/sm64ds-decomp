//cpp
// func_ov004_020b0104 at 0x020b0104
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov004).
extern "C" {
extern char* data_ov004_020beb68;
extern int data_ov004_020beb6c;
int _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int, void*, int, int, int, int, void*);
typedef int (*GetFn)(void*);
void func_ov004_020b0104(void* a0, int a1, int a2, int a3, int a4, void* a5){
  char* g = data_ov004_020beb68;
  if(g == 0) return;
  if(*(int*)(g+0x4628) == 0){
    void** vt = *(void***)g;
    GetFn f = (GetFn)vt[0x1a];
    if(f(g) == 2){
      char* g2 = data_ov004_020beb68;
      if(*(unsigned short*)(g2+0x4664) == 1){
        _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, a0, a1, a2 + 0xc0 + data_ov004_020beb6c, a3, a4, a5);
        return;
      }
      _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, a0, a1, a2, a3, a4, a5);
      return;
    }
  }
  _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, a0, a1, a2 + 0xc0 + data_ov004_020beb6c, a3, a4, a5);
  _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(1, a0, a1, a2, a3, a4, a5);
}
}
