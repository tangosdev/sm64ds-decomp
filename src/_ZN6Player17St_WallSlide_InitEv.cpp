//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

struct Player {
    int St_WallSlide_Init();
};

int Player::St_WallSlide_Init()
{
    char* c = (char*)this;
_ZN6Player7SetAnimEji5Fix12IiEj(c,0x29,0x40000000,0x1000,0);
  *(int*)(c+0x98)=0;
  if(*(int*)(c+0xa8) >= 0) *(int*)(c+0xa8)=0;
  *(short*)(c+0x6a4)=7;
  return 1;

}
