//cpp
extern "C" {
extern void func_ov002_020c9e40(char*c);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned,void*);
}

struct Player {
    int St_Teleport_Init();
};

int Player::St_Teleport_Init()
{
    char* c = (char*)this;
func_ov002_020c9e40(c);
  *(unsigned char*)(c+0x6e5)=0;
  *(unsigned char*)(c+0x6e3)=0;
  *(int*)(c+0x98)=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(c,0x47,0,0x1000,0);
  _ZN5Sound9PlayBank0EjRK7Vector3(0x19,(char*)c+0x74);
  return 1;

}
