extern int _ZN9Animation7AdvanceEv(void*);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void*);
extern int Scuttlebug_SetState(void*,int);
extern int _ZN5Actor9UpdatePosEP12CylinderClsn(void*,void*);
extern int func_ov071_0211f148(void*,void*);
extern int func_ov071_0211f29c(void*);
extern int _ZN12CylinderClsn5ClearEv(void*);
extern int _ZN12CylinderClsn6UpdateEv(void*);
int func_ov071_0211fc60(char* c){
  _ZN9Animation7AdvanceEv((char*)c+0x124);
  if(_ZNK12WithMeshClsn10IsOnGroundEv((char*)c+0x194)){
    *(short*)(c+0x3a8)=0x3c;
    Scuttlebug_SetState(c, 2);
  }
  _ZN5Actor9UpdatePosEP12CylinderClsn(c, (char*)c+0x160);
  func_ov071_0211f148(c, (char*)c+0x194);
  func_ov071_0211f29c(c);
  _ZN12CylinderClsn5ClearEv((char*)c+0x160);
  _ZN12CylinderClsn6UpdateEv((char*)c+0x160);
  return 1;
}
