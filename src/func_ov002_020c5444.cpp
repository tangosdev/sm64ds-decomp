//cpp
struct VecFx32 { int x, y, z; };
extern "C" {
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
}
extern "C" void func_ov002_020c5444(char* c){
  VecFx32 v;
  ((int*)&v)[0] = *(int*)(c + 0x5c);
  ((int*)&v)[1] = *(int*)(c + 0x60);
  ((int*)&v)[2] = *(int*)(c + 0x64);
  ((int*)&v)[1] = *(int*)(c + 0x60) + 0x14000;
  unsigned char b = *(unsigned char*)(c + 0x70c);
  if (b == 0) {
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xdb, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xdc, v.x, v.y, v.z);
    return;
  }
  if (b == 1) {
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd8, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd9, v.x, v.y, v.z);
    return;
  }
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xc7, v.x, v.y, v.z);
}
