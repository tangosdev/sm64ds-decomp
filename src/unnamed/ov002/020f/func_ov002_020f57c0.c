struct E {
  char p0[8];
  int f8;
  int fc;
  char p10[0x20];
  unsigned short f30;
  unsigned short f32;
  char p34[6];
  unsigned short f3a;
  char p3c[9];
  unsigned char f45;
  char p46;
  unsigned char f47;
  char p48;
  unsigned char f49;
  char p4a[2];
};
extern int data_ov002_02100170[];
void func_ov002_020f57c0(struct E* arr, int idx) {
  if (arr[idx].f3a != 0) return;
  if (arr[idx].f32 != 0) return;
  arr[idx].f49 = 1;
  arr[idx].f45 = 1;
  arr[idx].f47++;
  arr[idx].f8 = data_ov002_02100170[idx*2];
  arr[idx].fc = data_ov002_02100170[idx*2+1];
  if (idx == 0) arr[idx].f30 = 0x78;
  else arr[idx].f30 = 0x20;
}
