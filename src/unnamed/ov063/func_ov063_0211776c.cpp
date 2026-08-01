//cpp
extern "C" {
extern void func_ov063_0211adfc(char* p);
extern void func_ov063_02118ddc(char* c);
extern int _ZN9ActorBase18MarkForDestructionEv(char* c);
void func_ov063_0211776c(char* c){
  func_ov063_0211adfc(c);
  unsigned short v=*(unsigned short*)(c+0x100);
  if(v>0xf) return;
  if(v!=0xf) return;
  func_ov063_02118ddc(c);
  _ZN9ActorBase18MarkForDestructionEv(c);
}
}
