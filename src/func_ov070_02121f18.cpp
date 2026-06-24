//cpp
extern "C" {
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void func_ov070_02121c8c(void* c);
extern void* _ZN5Actor22ClosestNonVanishPlayerEv(void* c);
extern short Vec3_HorzAngle(void* v0, void* v1);
extern void _Z14ApproachLinearRsss(short* p, short t, short step);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void* c, void* cc);
extern void func_ov070_02121be4(void* c);
extern void func_ov070_02121d50(void* c, void* p);
extern void func_ov070_02121cbc(void* c);
extern void _ZN12CylinderClsn5ClearEv(void* cc);
extern void _ZN12CylinderClsn6UpdateEv(void* cc);
int func_ov070_02121f18(char* c) {
  char* p;
  if (DecIfAbove0_Byte((unsigned char*)(c + 0x32c)) == 0)
    func_ov070_02121c8c(c);
  p = (char*)_ZN5Actor22ClosestNonVanishPlayerEv(c);
  if (p) {
    short ang = Vec3_HorzAngle(c + 0x5c, p + 0x5c);
    _Z14ApproachLinearRsss((short*)(c + 0x8e), ang, 0x180);
    *(short*)(c + 0x94) = *(short*)(c + 0x8e);
  }
  _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0xfc);
  func_ov070_02121be4(c);
  func_ov070_02121d50(c, c + 0x130);
  func_ov070_02121cbc(c);
  _ZN12CylinderClsn5ClearEv(c + 0xfc);
  _ZN12CylinderClsn6UpdateEv(c + 0xfc);
  return 1;
}
}
