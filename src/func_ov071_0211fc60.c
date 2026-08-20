extern int _ZN9Animation7AdvanceEv(void*);
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void*);
extern int Scuttlebug_SetState(void*,int);
extern int _ZN8dActor_c9UpdatePosEP5dCc_c(void*,void*);
extern int func_ov071_0211f148(void*,void*);
extern int func_ov071_0211f29c(void*);
extern int _ZN5dCc_c5ClearEv(void*);
extern int _ZN5dCc_c6UpdateEv(void*);
int func_ov071_0211fc60(char* c){
  _ZN9Animation7AdvanceEv((char*)c+0x124);
  if(_ZNK10dBgCh_Actr10IsOnGroundEv((char*)c+0x194)){
    *(short*)(c+0x3a8)=0x3c;
    Scuttlebug_SetState(c, 2);
  }
  _ZN8dActor_c9UpdatePosEP5dCc_c(c, (char*)c+0x160);
  func_ov071_0211f148(c, (char*)c+0x194);
  func_ov071_0211f29c(c);
  _ZN5dCc_c5ClearEv((char*)c+0x160);
  _ZN5dCc_c6UpdateEv((char*)c+0x160);
  return 1;
}
