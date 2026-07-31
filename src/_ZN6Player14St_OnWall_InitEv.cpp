//cpp
extern "C" {
extern int data_ov002_020ff164[];
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
}

struct Player {
    int St_OnWall_Init();
};

int Player::St_OnWall_Init()
{
    char* c = (char*)this;
int r3=0x1000;
  if(*(int*)(c+8)==2){
    if(*(unsigned char*)(c+0x6e3)==2) r3=0x2000;
  }
  _ZN6Player7SetAnimEji5Fix12IiEj(c, data_ov002_020ff164[*(unsigned char*)(c+0x6e3)], 0, r3, 0);
  *(int*)(c+0x98)=0;
  return 1;

}
