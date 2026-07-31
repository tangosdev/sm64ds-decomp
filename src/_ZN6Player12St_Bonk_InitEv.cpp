//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

struct Player {
    int St_Bonk_Init();
};

int Player::St_Bonk_Init()
{
    char* c = (char*)this;
_ZN6Player7SetAnimEji5Fix12IiEj(c,0x13,0x40000000,0x1000,0);
  *(short*)(c+0x6a4)=0x10;
  *(int*)(c+0x98)=0x40;
  *(int*)(c+0xa8)=0xa000;
  *(char*)(c+0x6e5)=0;
  return 1;

}
