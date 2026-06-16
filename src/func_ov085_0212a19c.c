extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void*, void*);
extern int func_ov085_02129ebc(void*, void*);
extern int func_ov085_02129f8c(void*);
int func_ov085_0212a19c(char* c){
  short v = *(short*)(c+0x8e);
  *(short*)(c+0x94) = v;
  _ZN5Actor9UpdatePosEP12CylinderClsn(c, c+0x160);
  func_ov085_02129ebc(c, c+0x194);
  func_ov085_02129f8c(c);
  return 1;
}
