//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

struct Player {
    int St_TurnAround_Init();
};

int Player::St_TurnAround_Init()
{
    char* c = (char*)this;
_ZN6Player7SetAnimEji5Fix12IiEj(c,0x46,0x40000000,0x1000,0);
  *(char*)(c+0x6e3)=0;
  return 1;

}
