extern int data_ov006_0212e088[];
struct S2 { int w[2]; };
extern struct S2 data_ov006_0213b354;
void func_ov006_020cdeec(char *c) {
  int idx = *(short *)(c + 0x96);
  *(int *)(c + 0x2c) = data_ov006_0212e088[idx];
  *(struct S2 *)c = data_ov006_0213b354;
}
