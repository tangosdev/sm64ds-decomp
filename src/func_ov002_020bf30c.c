typedef int Fix12i;
extern unsigned short data_ov002_020ff170[];
Fix12i func_ov002_020bf30c(void*c, Fix12i a){
  int idx = *(int*)((char*)c+8) & 0xff;
  unsigned int t = data_ov002_020ff170[idx];
  return (Fix12i)(((long long)a * (int)t + 0x800) >> 12);
}
