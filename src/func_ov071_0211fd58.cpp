//cpp
extern "C" {
int _ZN9Animation7AdvanceEv(void*);
void func_ov071_021202ec(void*, int, void*);
int _ZN5Actor9UpdatePosEP12CylinderClsn(void*, void*);
int func_ov071_0211f148(void*, void*);
int func_ov071_0211f29c(void*);
int _ZN12CylinderClsn5ClearEv(void*);
int _ZN12CylinderClsn6UpdateEv(void*);
int _ZNK12WithMeshClsn10IsOnGroundEv(void*);
int func_0201267c(int, void*);
int func_ov071_0211fd58(char* c){
  _ZN9Animation7AdvanceEv(c+0x124);
  int* p = (int*)(((int)c + 0x3a0) & 0xFFFFFFFFFFFFFFFF);
  *p -= 0xf000;
  if(*(int*)(c+0x3a0) <= 0){
    char* b = c+0x300;
    *(short*)(b+0xa8) = 0x3c;
    func_ov071_021202ec(c, 2, b);
  }
  _ZN5Actor9UpdatePosEP12CylinderClsn(c, c+0x160);
  func_ov071_0211f148(c, c+0x194);
  func_ov071_0211f29c(c);
  _ZN12CylinderClsn5ClearEv(c+0x160);
  _ZN12CylinderClsn6UpdateEv(c+0x160);
  if(_ZNK12WithMeshClsn10IsOnGroundEv(c+0x194) != 0){
    unsigned int t = (unsigned int)(*(int*)(c+0x12c) << 4) >> 0x10;
    if((t <= 2) || (t >= 8 && t <= 0xa) || (t >= 0x18 && t <= 0x1a) || (t >= 0x20 && t <= 0x22)){
      func_0201267c(0xf0, c+0x74);
    }
  }
  return 1;
}
}
