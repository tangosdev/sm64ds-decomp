//cpp
extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void* fp);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
}
extern void* data_ov002_0210d9d8;
extern void* data_ov002_0210da30;
extern "C" int _ZN13OneUpMushroom16CleanupResourcesEv(char* c){
  int s = *(int*)(c + 0x384);
  if (s != 0xb && s != 0xc){
    int b = (*(unsigned short*)(c + 0xc) == 0x114);
    if (b != 0) _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210d9d8);
    else _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210da30);
  }
  if ((unsigned int)(*(int*)(c + 0x384) - 0xb) <= 1)
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, *(int*)(c + 0x5c), *(int*)(c + 0x60) + 0x28000, *(int*)(c + 0x64));
  return 1;
}
