//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern struct Camera* data_0209f318;
extern void func_0200d544(struct Camera* thiz, unsigned char playerID);
}

struct Player {
    int St_StomachSlide_Init();
};

int Player::St_StomachSlide_Init()
{
    char* c = (char*)this;
_ZN6Player7SetAnimEji5Fix12IiEj(c, 0x40, 0x40000000, 0x1000, 0);
  *(char*)(c+0x6e3)=0;
  if(*(unsigned char*)(c+0x70e)){
    func_0200d544(data_0209f318, *(unsigned char*)(c+0x6d8));
  }
  *(char*)(c+0x6e5)=0;
  *(char*)(c+0x6e6)=0;
  *(char*)(c+0x6e7)=0;
  *(char*)(c+0x70c)=*(unsigned char*)(c+0x70e);
  *(char*)(c+0x6e4)=0;
  return 1;

}
