//cpp
extern "C" {
extern int func_ov002_020dab14(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int func_ov002_020bd928(void*,int);
}

struct Player {
    int St_Shell_Init();
};

int Player::St_Shell_Init()
{
    char* c = (char*)this;
func_ov002_020dab14(c);
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x35,0x40000000,0x1000,0);
  *(char*)(c+0x6e3)=0;
  func_ov002_020bd928(c,0x33);
  return 1;

}
