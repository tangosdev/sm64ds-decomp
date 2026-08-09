//cpp
extern "C" {
extern int DecIfAbove0_Byte(void*);
extern void func_ov027_021123b0(void*);
extern void func_ov027_0211233c(void*);
extern void func_ov027_02112170(void*);
extern void _ZN12CylinderClsn5ClearEv(void*);
extern void _ZN12CylinderClsn6UpdateEv(void*);
void func_ov027_02112480(char* c){
  if(DecIfAbove0_Byte(c+0x5e)==0) return;
  func_ov027_021123b0(c);
  func_ov027_0211233c(c);
  func_ov027_02112170(c);
  *(int*)(c+0x30)=*(int*)(c+0x40);
  *(int*)(c+0x34)=*(int*)(c+0x44);
  *(int*)(c+0x38)=*(int*)(c+0x48);
  _ZN12CylinderClsn5ClearEv(c);
  _ZN12CylinderClsn6UpdateEv(c);
}
}
