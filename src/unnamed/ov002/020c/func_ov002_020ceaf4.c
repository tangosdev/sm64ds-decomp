extern int data_0209f32c;
extern int data_ov002_020ff1e0[];
int func_ov002_020ceaf4(char* c){
  int v;
  if(data_0209f32c - 0x50000 - *(int*)(c+0x60) < 0x1f4000) return 0x1800;
  v = data_ov002_020ff1e0[*(int*)(c+8)];
  return -(int)((((long long)v << 13) + 0x800) >> 12);
}
