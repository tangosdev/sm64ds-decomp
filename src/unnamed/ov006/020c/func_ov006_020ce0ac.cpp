//cpp
extern int ApproachLinear(int &, int, int);
extern int data_ov006_02140838;
extern int data_ov006_02140828;
struct S2 { int w[2]; };
extern struct S2 data_ov006_0213b344;
extern "C" void func_ov006_020ce0ac(char *c) {
  ApproachLinear(data_ov006_02140828, data_ov006_02140838, 1);
  *(char *)(c + 0x9c) = 0x1f;
  *(int *)(c + 0x2c) = 0;
  *(int *)(c + 0x30) = 0;
  *(int *)(c + 0x34) = 0;
  *(struct S2 *)c = data_ov006_0213b344;
}
