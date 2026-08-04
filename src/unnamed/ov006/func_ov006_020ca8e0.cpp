//cpp
extern int data_ov006_021405b8;
extern int data_ov006_021405bc;
extern char* data_ov006_02140554;
extern int data_ov006_0213b104[2];
extern int data_ov006_021405a4;
extern "C" void _Z14ApproachLinearRiii(int& ref, int to, int step);
extern "C" void func_ov006_020ca374(char* o, short v);
extern "C" void func_ov006_020c8ecc(char* o);

extern "C" void func_ov006_020ca8e0(void) {
  char* base;
  char* e;
  int i;
  int q0;
  int count;

  _Z14ApproachLinearRiii(data_ov006_021405bc, data_ov006_021405b8, 1);
  i = 0;
  count = data_ov006_021405bc;
  if (count <= 0)
    return;

  base = data_ov006_02140554;
  e = base;
  q0 = data_ov006_0213b104[0];
  for (;;) {

    int* v = (int*)(((long long)(int)(e + 0x70)));
    volatile int* q = (volatile int*)data_ov006_0213b104;
    if (v[0] == q0 && (v[1] == q[1] || *(int*)(e + 0x70) == 0)) {

      func_ov006_020ca374(base + i * 0xdc, (short)(i * 0x14));
      *(int*)(data_ov006_02140554 + i * 0xdc + 0x24) = 0;
      *(int*)(data_ov006_02140554 + i * 0xdc + 0x3c) =
        (int)(((long long)data_ov006_021405a4 * ((i << 11) - 0x1000) + 0x800) >> 12);
      func_ov006_020c8ecc(data_ov006_02140554 + i * 0xdc);
      return;

    }

    i++;
    e += 0xdc;
    if (i < count) continue;
    break;
  }

}
