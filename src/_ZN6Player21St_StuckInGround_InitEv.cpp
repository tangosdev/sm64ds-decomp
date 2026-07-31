//cpp
extern "C" {
extern int func_ov002_020c9e40(void*);
extern int func_ov002_020dab14(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned int,int,int,unsigned int);
extern int _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int,void*);
extern int func_ov002_020c5444(void*);
extern int data_ov002_0210a560[];
}

struct Player {
    int St_StuckInGround_Init();
};

int Player::St_StuckInGround_Init()
{
    void* c = (void*)this;
func_ov002_020c9e40(c);
  func_ov002_020dab14(c);
  unsigned char idx = *(unsigned char*)((char*)c+0x6e3);
  _ZN6Player7SetAnimEji5Fix12IiEj(c, data_ov002_0210a560[idx], 0x40000000, 0x1000, 0);
  _ZN5Sound9PlayBank0EjRK7Vector3(*(int*)((char*)c+0x66c)+0x80, (char*)c+0x74);
  func_ov002_020c5444(c);
  *(int*)((char*)c+0xa8)=0;
  *(int*)((char*)c+0x98)=0;
  *(unsigned char*)((char*)c+0x6e5)=0;
  return 1;

}
