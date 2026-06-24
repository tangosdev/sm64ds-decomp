extern void func_ov004_020b0104(void* a0, int a1, int a2, int a3, int a4, int a5);
extern void func_ov004_020af868(void* a0, int a1, int a2, int a3, int a4, int a5);
extern void func_ov004_020afc18(void* a0, int a1, int a2, int a3, int a4, int a5);
void func_ov004_020b1d60(void* a0, int a1, int a2, int a3, int a4){
  if(a3==-1){
    func_ov004_020b0104(a0, a1, a2, -1, a4, 0);
    return;
  }
  if(a3==0){
    func_ov004_020af868(a0, a1, a2, -1, a4, 0);
    return;
  }
  func_ov004_020afc18(a0, a1, a2, -1, a4, 0);
}
