//cpp
extern "C" {
extern void func_02054154(void);
extern void func_02054140(void);
void _ZN2GX16SetBankForSubOBJEt(unsigned short);
void _ZN2GX15SetBankForSubBGEt(unsigned short);
void func_ov006_020e73c4(void){
  func_02054154();
  func_02054140();
  _ZN2GX16SetBankForSubOBJEt(8);
  _ZN2GX15SetBankForSubBGEt(4);
  *(unsigned int*)0x4001000 &= ~0x300010;
}
}
