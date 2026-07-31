//cpp
struct Player {
    int St_HoldLight_Cleanup();
};

int Player::St_HoldLight_Cleanup()
{
    char* c = (char*)this;
char* p = *(char**)(c + 0x358);
  if (p) {
    *(unsigned int*)(((long long)(int)(p + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x4000;
  }
  return 1;

}
