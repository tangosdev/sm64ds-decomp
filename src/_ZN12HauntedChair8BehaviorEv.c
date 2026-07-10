extern void func_0200f760(char* a, char* b);
extern void func_ov020_021130c8(char* c);
extern void func_ov020_02112e94(char* c);
extern void func_ov020_02112b00(char* c);
extern void func_ov020_021129dc(char* c);
extern void func_ov020_02113240(char* c);
extern void _ZN12CylinderClsn5ClearEv(char* c);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(char* c, char* v);
extern void _ZN12CylinderClsn6UpdateEv(char* c);

int _ZN12HauntedChair8BehaviorEv(char* r4){
  func_0200f760(r4, r4 + 0x17c);
  switch (*(int*)(r4 + 0x378)) {
  case 0: func_ov020_021130c8(r4); break;
  case 1: func_ov020_02112e94(r4); break;
  case 2: func_ov020_02112b00(r4); break;
  case 3: func_ov020_021129dc(r4); break;
  }
  func_ov020_02113240(r4);
  _ZN12CylinderClsn5ClearEv(r4 + 0x17c);
  _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(r4 + 0x17c, r4 + 0x38c);
  _ZN12CylinderClsn6UpdateEv(r4 + 0x17c);
  return 1;
}
