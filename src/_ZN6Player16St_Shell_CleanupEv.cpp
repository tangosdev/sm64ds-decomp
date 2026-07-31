//cpp
extern "C" {
extern void func_ov002_020bd8c0(char* c, unsigned int r1);
}

struct Player {
    int St_Shell_Cleanup();
};

int Player::St_Shell_Cleanup()
{
    char* c = (char*)this;
char* p = *(char**)(c + 0x354);
  if (p != 0) {
    *(int*)(p + 0x3c0) = 0;
    *(int*)(c + 0x354) = 0;
    *(int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFFLL) += 0x46000;
    *(unsigned char*)(c + 0x6de) = 1;
    *(unsigned char*)(c + 0x6df) = 0;
    func_ov002_020bd8c0(c, 0x33);
  }
  return 1;

}
