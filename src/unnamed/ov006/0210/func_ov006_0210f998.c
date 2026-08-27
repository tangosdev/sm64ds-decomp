struct W9 { int w[9]; };
extern struct W9 data_ov006_0213eed0;
extern int data_ov006_02142c1c[9];
void func_ov006_0210f998(void){
  struct W9 tmp;
  int i;
  tmp = data_ov006_0213eed0;
  for (i = 0; i < 9; i++) {
    data_ov006_02142c1c[i] = tmp.w[i] - 1;
  }
}
