//cpp
extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void* fp);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
}
extern void* ONE_UP_MUSHROOM_MODEL_PTR;
extern void* SUPER_MUSHROOM_MODEL_PTR;
extern "C" int func_ov002_020affe8(char* c){
  int s = *(int*)(c + 0x384);
  if (s != 0xb && s != 0xc){
    int b = (*(unsigned short*)(c + 0xc) == 0x114);
    if (b != 0) _ZN13SharedFilePtr7ReleaseEv(&ONE_UP_MUSHROOM_MODEL_PTR);
    else _ZN13SharedFilePtr7ReleaseEv(&SUPER_MUSHROOM_MODEL_PTR);
  }
  if ((unsigned int)(*(int*)(c + 0x384) - 0xb) <= 1)
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, *(int*)(c + 0x5c), *(int*)(c + 0x60) + 0x28000, *(int*)(c + 0x64));
  return 1;
}
