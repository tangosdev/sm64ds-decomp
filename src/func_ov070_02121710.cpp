//cpp
extern "C" {
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void func_ov070_02121880(void* c, int a);
extern void _ZN9Animation7AdvanceEv(void* a);
extern int func_ov070_02121a64(void* p);
extern void func_ov070_02121298(char* c);
extern void func_ov070_021211c4(char* c);
extern void _ZN12CylinderClsn5ClearEv(void* cc);
extern void _ZN12CylinderClsn6UpdateEv(void* cc);
extern int data_0209f32c;
int func_ov070_02121710(char* c) {
  int r;
  if (*(int*)(c + 0x3a4) != 0) {
    if (*(int*)(c + 0x60) > data_0209f32c) {
      if (DecIfAbove0_Byte((unsigned char*)(c + 0x3ac)) == 0)
        func_ov070_02121880(c, 1);
    }
  } else {
    *(unsigned char*)(c + 0x3ac) = 0x73;
  }
  _ZN9Animation7AdvanceEv(c + 0x124);
  r = func_ov070_02121a64(c + 0x38c);
  *(int*)(c + 0x80) = r;
  *(int*)(c + 0x84) = r;
  *(int*)(c + 0x88) = r;
  func_ov070_02121298(c);
  func_ov070_021211c4(c);
  _ZN12CylinderClsn5ClearEv(c + 0x160);
  _ZN12CylinderClsn6UpdateEv(c + 0x160);
  return 1;
}
}
