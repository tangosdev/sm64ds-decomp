//cpp
extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void* p);
int SharedFilePtr_Destruct_Anim(int x);
int func_02017ab4(int x);
void _ZN9ModelAnimD1Ev(void* p);
void* __destroy_arr(void* p, int a, int b, void* f);
}
extern void* func_ov006_020c3e54;
extern "C" void* func_ov006_020c3e70(char* c){
  _ZN13SharedFilePtr7ReleaseEv(c + 0xd7c);
  _ZN13SharedFilePtr7ReleaseEv(c + 0xd84);
  _ZN13SharedFilePtr7ReleaseEv(c + 0xd8c);
  _ZN13SharedFilePtr7ReleaseEv(c + 0xd94);
  _ZN13SharedFilePtr7ReleaseEv(c + 0xd9c);
  _ZN13SharedFilePtr7ReleaseEv(c + 0xda4);
  SharedFilePtr_Destruct_Anim((int)(c + 0xda4));
  SharedFilePtr_Destruct_Anim((int)(c + 0xd9c));
  SharedFilePtr_Destruct_Anim((int)(c + 0xd94));
  SharedFilePtr_Destruct_Anim((int)(c + 0xd8c));
  func_02017ab4((int)(c + 0xd84));
  func_02017ab4((int)(c + 0xd7c));
  _ZN9ModelAnimD1Ev(c + 0xd18);
  __destroy_arr(c + 8, 0x16, 0x98, &func_ov006_020c3e54);
  return c;
}
