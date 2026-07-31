//cpp
extern "C" {
extern int func_ov002_020da9d4(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,void*);
}

struct Player {
    int St_Electrocute_Init();
};

int Player::St_Electrocute_Init()
{
    void* c = (void*)this;
func_ov002_020da9d4(c);
  *(char*)((char*)c+0x708)=1;
  *(short*)((char*)c+0x6a4)=0x1e;
  *(short*)((char*)c+0x6a0)=0x1e;
  *(int*)((char*)c+0x98)=0;
  *(int*)((char*)c+0xa8)=0;
  *(int*)((char*)c+0x9c)=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x11,0,0x1000,0);
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)((char*)c+0x6d9),7,(char*)c+0x74);
  return 1;

}
