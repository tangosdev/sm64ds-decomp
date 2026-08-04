extern void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
extern void func_ov004_020ae274(void* c);
extern void func_ov006_02111d4c(char *p);

void func_ov006_02114dfc(char *c){
  int i;
  *(int*)(c + 0x5958) = 0;
  *(unsigned char*)(c + 0x595c) = 1;
  *(unsigned char*)(c + 0x595d) = 1;
  func_ov004_020b0cac(6, 0x80, -0x60, -1, -1, 0xd);
  func_ov004_020ae274(0);
  for(i = 0; i < *(int*)(c + 0x4668); i++){
    if(*(unsigned char*)(((i >= 0xd) ? 0 : ((char**)(c + 0x4688))[i]) + 0x30))
      func_ov006_02111d4c((i >= 0xd) ? 0 : ((char**)(c + 0x4688))[i]);
  }
}
