//cpp
extern "C" {
extern int data_ov002_02109fe4[];
extern int data_0209f318[];
extern void _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
extern void func_ov002_020d93ac(void*);
extern void func_ov002_020d94cc(void*);
extern void func_0200d89c(char* p);
int _ZN6Player17St_HurtWater_InitEv(char* c){
  unsigned a=data_ov002_02109fe4[*(unsigned char*)(c+0x6e3)&1];
  _ZN6Player7SetAnimEji5Fix12IiEj(c,a,0x40000000,0x1000,0);
  *(char*)(c+0x6e5)=0;
  *(char*)(c+0x70c)=0;
  *(int*)(c+0x9c)=0;
  *(int*)(c+0xa0)=-0xc000;
  *(int*)(c+0xa8)=0x10000;
  *(char*)(c+0x708)=1;
  if(*(int*)(c+0x674)){
    int b=*(unsigned char*)(c+0x6e3)&0xf0;
    if(b==0 || b==0x10){
      func_ov002_020d93ac(c);
      func_ov002_020d94cc(c);
    }
  }
  func_0200d89c(*(char**)data_0209f318);
  return 1;
}
}
