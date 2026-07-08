//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
extern int data_020ff118[];

void func_ov002_020c9c4c(char* c){
  _ZN6Player7SetAnimEji5Fix12IiEj(c, data_020ff118[*(unsigned char*)(c+0x6e3)], 0x40000000, 0x1000, 0);
  *(unsigned char*)(((long long)(int)(c+0x6e3)) & 0xFFFFFFFFFFFFFFFFLL) += 7;
  *(unsigned char*)(c+0x743) = 1;
  *(unsigned char*)(c+0x716) = 1;
  *(unsigned char*)(c+0x713) = 0;
  *(int*)(c+0x9c) = 0;
  *(int*)(c+0x98) = 0;
  *(int*)(c+0xa8) = 0;
}
}
