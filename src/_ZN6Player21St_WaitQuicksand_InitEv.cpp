//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

struct Player {
    int St_WaitQuicksand_Init();
};

int Player::St_WaitQuicksand_Init()
{
    char* c = (char*)this;
_ZN6Player7SetAnimEji5Fix12IiEj(c,0x47,0,0x1000,0);
  *(char*)(c+0x6e5)=0;
  return 1;

}
