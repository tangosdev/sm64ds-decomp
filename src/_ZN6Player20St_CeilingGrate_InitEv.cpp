//cpp
extern "C" {
extern void func_ov002_020cf384(char* p);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

struct Player {
    int St_CeilingGrate_Init();
};

int Player::St_CeilingGrate_Init()
{
    char* c = (char*)this;
func_ov002_020cf384(c);
  *(char*)(c+0x6e3)=0;
  *(char*)(c+0x6e5)=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x5b,0x40000000,0x1000,0);
  return 1;

}
