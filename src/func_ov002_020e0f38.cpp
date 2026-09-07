//cpp
extern "C" {
extern void _ZN8dActor_c11LandingDustEb(void* c, int b);
extern void func_0201267c(int anim, char* p);
extern int data_ov002_020ff17c[];
extern int data_ov002_020ff188[];
void func_ov002_020e0f38(char* c, int a){
  *(int*)(c+0xa8) = data_ov002_020ff17c[a];
  *(int*)(c+0x98) = data_ov002_020ff188[a];
  if (a != 0) _ZN8dActor_c11LandingDustEb(c, 1);
  if (a != 2) func_0201267c(0x6c, c+0x74);
  else func_0201267c(0x6d, c+0x74);
}
}
