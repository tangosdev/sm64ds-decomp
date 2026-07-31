//cpp
extern "C" {
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, int, unsigned int);
}

struct Player {
    int St_SwingPlayer_Init();
};

int Player::St_SwingPlayer_Init()
{
    char* c = (char*)this;
*(int*)(c+0x98) = 0;
  _ZN6Player7SetAnimEji5Fix12IiEj(c, 0x6c, 0x40000000, 0x1000, 0);
  *(unsigned char*)(c+0x6e3) = 0;
  *(short*)(c+0x69c) = *(short*)(c+0x8e) - *(short*)(c+0x6d6);
  {
    void* obj = *(void**)(c+0x358);
    if (obj != 0)
      *(int*)(((long long)(int)((char*)obj+0xb0)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x800;
  }
  return 1;

}
