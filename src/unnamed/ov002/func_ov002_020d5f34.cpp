//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern char data_ov002_02110064[];
int func_ov002_020d5f34(char* c, void* st){
  if (*(int*)(c+0xd0) != 0)
    return 0;
  *(void**)(c+0xd0) = st;
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x3f, 0, 0x3000, 0);
  *(unsigned char*)(c+0x6e3) = 0;
  _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_02110064);
  return 1;
}
}
