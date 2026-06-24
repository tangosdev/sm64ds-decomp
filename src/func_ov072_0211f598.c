typedef int Fix12;
struct Vector3 { int x, y, z; };
extern void _Z14ApproachLinearRiii(int* p, int a, int b);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int a, int b, int c);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, const struct Vector3* v, unsigned int d);
extern void func_ov072_0211f158(char* c);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(char* c, void* cc);
extern void func_ov072_0211f330(char* c, void* mc);
extern void func_ov072_0211f280(char* c);
extern void func_ov072_0211fcb0(char* c, int i);

int func_ov072_0211f598(char* c) {
  _Z14ApproachLinearRiii((int*)(c+0x98), 0x28000, 0x400);
  _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
  *(int*)(c+0x39c) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int*)(c+0x39c), 3, 0x8a, (struct Vector3*)(c+0x74), 0);
  func_ov072_0211f158(c);
  _ZN5Actor9UpdatePosEP12CylinderClsn(c, c+0x14c);
  func_ov072_0211f330(c, c+0x180);
  func_ov072_0211f280(c);
  if (*(int*)(c+0x60) < (int)0xfe363c80) {
    func_ov072_0211fcb0(c, 5);
  }
  return 1;
}
