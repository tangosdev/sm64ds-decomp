typedef int Fix12i;
extern unsigned short data_ov002_020ff140[];
void func_ov002_020bf2d8(void*c, Fix12i a){
  unsigned int t = data_ov002_020ff140[*(int*)((char*)c+8)];
  *(int*)((char*)c+0xa8) = (Fix12i)(((long long)a * (int)t + 0x800) >> 12);
}
