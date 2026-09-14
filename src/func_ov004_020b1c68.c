struct M { int _00, _01, _10, _11; };
extern void RenderOamBothScreens(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern void func_ov004_020af868(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern void func_ov004_020afc18(void* a0, int a1, int a2, int a3, int a4, void* a5);
void func_ov004_020b1c68(void* a0, int a1, int a2, int a3, int a4, struct M* a5){
  if(a3==-1){
    RenderOamBothScreens(a0, a1, a2, -1, a4, a5);
    return;
  }
  if(a3==0){
    func_ov004_020af868(a0, a1, a2, -1, a4, a5);
    return;
  }
  func_ov004_020afc18(a0, a1, a2, -1, a4, a5);
}
