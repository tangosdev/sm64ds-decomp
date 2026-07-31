//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

struct Player {
    int St_Crawl_Init();
};

int Player::St_Crawl_Init()
{
    char* c = (char*)this;
*(char*)(c+0x6e3)=3;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x64,0x40000000,0x1000,0);
  return 1;

}
