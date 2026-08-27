extern unsigned short* data_ov007_02104c1c;
extern char* data_ov007_02104c18;
extern int data_ov007_02104c24;
int func_ov007_020c94a0(int a, unsigned int b){
  unsigned short v = data_ov007_02104c1c[5];
  unsigned int idx = (unsigned int)(v * (b & 0xffff));
  return data_ov007_02104c24 + *(int*)(data_ov007_02104c18 + (idx >> 3));
}
