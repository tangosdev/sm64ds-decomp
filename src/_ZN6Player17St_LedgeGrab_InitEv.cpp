//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,void*);
}

struct Player {
    int St_LedgeGrab_Init();
};

int Player::St_LedgeGrab_Init()
{
    char* c = (char*)this;
if(*(unsigned char*)(c+0x6e3)){
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x20,0x40000000,0x1000,0);
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9),0x1a,(void*)(c+0x74));
  } else {
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x23,0x40000000,0x1000,0);
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9),0x17,(void*)(c+0x74));
  }
  return 1;

}
