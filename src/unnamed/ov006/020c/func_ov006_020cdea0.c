extern int data_ov006_0212e07c[];
extern int data_ov006_0212e088[];
void func_ov006_020cdea0(char *c) {
  int idx = *(short *)(c + 0x96);
  if (*(int *)(c + 8) > data_ov006_0212e07c[idx]) {
    *(int *)(c + 0x2c) = -data_ov006_0212e088[idx];
    return;
  }
  if (*(int *)(c + 8) < -data_ov006_0212e07c[idx]) {
    *(int *)(c + 0x2c) = data_ov006_0212e088[idx];
  }
}
