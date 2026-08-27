//cpp
extern "C" {
typedef int Fix12i;
struct Vector3 { int x, y, z; ~Vector3(){} };
extern void func_ov002_020bd984(void* c, unsigned int r1);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, Fix12i a, Fix12i b, Fix12i d);
extern void _ZN5Sound9PlayBank0EjRK7Vector3(unsigned int id, Vector3* v);
void func_ov002_020de968(char* c){
  *(short*)(c+0x6c0) = 0;
  if (*(unsigned char*)(c+0x6fd) != 1) return;
  *(unsigned char*)(c+0x6fd) = 0;
  *(unsigned char*)(c+0x6f5) = 0x1f;
  func_ov002_020bd984(c, 0x30);
  Vector3 v;
  v.x = *(int*)(c+0x5c);
  v.y = *(int*)(c+0x60);
  v.z = *(int*)(c+0x64);
  v.y = *(int*)(c+0x60) + 0x50000;
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xc5, v.x, v.y, v.z);
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xc6, v.x, v.y, v.z);
  _ZN5Sound9PlayBank0EjRK7Vector3(0xd8, (Vector3*)(c+0x74));
}
}
