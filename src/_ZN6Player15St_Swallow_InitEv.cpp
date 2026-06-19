//cpp
extern "C" {
struct State; extern State data_ov002_02110034;
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,State*);
extern int func_ov002_020c9e40(void*);
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
int _ZN6Player15St_Swallow_InitEv(char* c){
  if(*(void**)(c+0x360)){
    void* o=*(void**)(c+0x360);
    int(*f)(void*)=*(int(**)(void*))(*(char**)o+0x48);
    if(f(o)==4){
      func_ov002_020c9e40(c);
    }
    int eq = *(unsigned short*)(*(char**)(c+0x360)+0xc)==0xbf;
    if(eq!=0){
      _ZN6Player11ChangeStateERNS_5StateE(c,&data_ov002_02110034);
      return 1;
    }
  }
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x70,0x40000000,0x1000,0);
  return 1;
}
}
