//cpp
extern "C" {
extern int data_0209f318[];
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern void func_0200d6b4(void*,unsigned char);
extern void func_ov002_020bd928(char*,unsigned int);
}

struct Player {
    int St_Owl_Init();
};

int Player::St_Owl_Init()
{
    char* c = (char*)this;
*(int*)(c+0xa0)=-0x4b000;
  *(int*)(c+0x9c)=0;
  *(unsigned char*)(c+0x6e3)=0;
  *(int*)(c+0x98)=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x5b,0x40000000,0x1000,0);
  func_0200d6b4((void*)data_0209f318[0], *(unsigned char*)(c+0x6d8));
  func_ov002_020bd928(c,0x2f);
  return 1;

}
