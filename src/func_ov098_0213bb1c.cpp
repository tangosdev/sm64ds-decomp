//cpp
extern "C" {
extern int _ZNK10dBgCh_Actr10IsOnGroundEv(void* p);
extern void func_ov098_0213b520(void* c);
extern void func_ov098_0213b7e8(void* c);
extern void _ZN7fBase_c18MarkForDestructionEv(void* c);
void func_ov098_0213bb1c(char* c){
  if(*(int*)(c+0x3c8)!=3){
    if(_ZNK10dBgCh_Actr10IsOnGroundEv(c+0x144)==0) return;
    if(*(int*)(c+0x3c8)==1){
      *(int*)(c+0x3c4)=2;
      func_ov098_0213b520(c);
      func_ov098_0213b7e8(c);
      return;
    }
    _ZN7fBase_c18MarkForDestructionEv(c);
    return;
  }
  if(*(int*)(c+0x60) < 0x1800000) return;
  _ZN7fBase_c18MarkForDestructionEv(c);
}
}
