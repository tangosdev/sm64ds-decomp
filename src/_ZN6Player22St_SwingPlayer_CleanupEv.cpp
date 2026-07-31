//cpp
struct Player {
    int St_SwingPlayer_Cleanup();
};

int Player::St_SwingPlayer_Cleanup()
{
    char* c = (char*)this;
char* p = *(char**)(c + 0x358);
  if (p) {
    *(unsigned int*)(((long long)(int)(p + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x800;
  }
  *(short*)(c + 0x94) = *(short*)(c + 0x8e);
  return 1;

}
