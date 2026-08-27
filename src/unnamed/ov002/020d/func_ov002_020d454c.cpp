//cpp
extern "C" {
extern int Player_ScaleByCharFactor(void* c, int a);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
void func_ov002_020d454c(char* c){
  if (*(unsigned char*)(c+0x6de) != 0) return;
  if (*(int*)(c+0x98) > Player_ScaleByCharFactor(c, 0x10000))
    *(int*)(c+0x98) = Player_ScaleByCharFactor(c, 0x10000);
  *(unsigned char*)(c+0x6e0) = 1;
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x36, 0x40000000, 0x1000, 0);
}
}
