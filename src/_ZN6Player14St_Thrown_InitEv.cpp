//cpp
extern "C" {
struct Vector3{int x,y,z;};
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned a, unsigned b, const Vector3& v);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
}

struct Player {
    int St_Thrown_Init();
};

int Player::St_Thrown_Init()
{
    char* c = (char*)this;
int b = (*(int*)(c+0xb0) & 0x400) ? 1 : 0;
  if(b)
    *(short*)(c+0x6a4)=4;
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 0x1d, *(Vector3*)(c+0x74));
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x15,0x40000000,0x1000,0x12);
  *(unsigned char*)(c+0x6e3)=0;
  *(unsigned char*)(c+0x6e5)=0;
  *(short*)(c+0x8e)=*(short*)(c+0x94);
  *(unsigned char*)(c+0x70c)=0;
  return 1;

}
