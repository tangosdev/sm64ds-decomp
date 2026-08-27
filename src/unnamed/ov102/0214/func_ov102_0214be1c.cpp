//cpp
extern "C" {
extern void _Z14ApproachLinearRsss(short* p, short t, short step);
extern void _ZN9Animation7AdvanceEv(void* a);
extern void func_ov102_0214c0b8(void* c);
void func_ov102_0214be1c(char* c) {
  if (*(int*)(c + 0x38c) == 0) {
    *(int*)(c + 0x35c) = 0x800;
    _Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x3ee), 0x400);
  } else {
    *(int*)(c + 0x35c) = 0x1000;
    _Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x3ee), 0x800);
  }
  *(short*)(c + 0x8e) = *(short*)(c + 0x94);
  _ZN9Animation7AdvanceEv(c + 0x350);
  if (*(int*)(c + 0x3dc) == 2) return;
  if (*(short*)(c + 0x94) != *(short*)(c + 0x3ee)) return;
  func_ov102_0214c0b8(c);
}
}
