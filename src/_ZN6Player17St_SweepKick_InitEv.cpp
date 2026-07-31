//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int,unsigned int,struct Vector3*);
}

struct Player {
    int St_SweepKick_Init();
};

int Player::St_SweepKick_Init()
{
    char* c = (char*)this;
_ZN6Player7SetAnimEji5Fix12IiEj(c, 0x68, 0x40000000, 0x1000, 0);
  *(int*)(c+0x98)=0;
  *(char*)(c+0x726)=0;
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 0xa, (struct Vector3*)(c+0x74));
  return 1;

}
