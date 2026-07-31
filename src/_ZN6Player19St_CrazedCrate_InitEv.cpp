//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
extern void func_ov002_020e0f38(void* c, unsigned char a);
}

struct Player {
    int St_CrazedCrate_Init();
};

int Player::St_CrazedCrate_Init()
{
    char* c = (char*)this;
*(unsigned char*)(c+0x71b) = 0;
  *(unsigned char*)(c+0x712) = 1;
  *(unsigned char*)(c+0x70d) = 0;
  *(unsigned char*)(c+0x6e1) = 0;
  *(unsigned char*)(c+0x6de) = 1;
  *(unsigned char*)(c+0x6df) = 0;
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x43, 0x40000000, 0x1000, 0);
  func_ov002_020e0f38(c, *(unsigned char*)(c+0x6e1));
  *(unsigned short*)(c+0x94) = *(short*)(c+0x8e);
  *(int*)(((long long)(int)(c + 0x2ec)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x20;
  return 1;

}
