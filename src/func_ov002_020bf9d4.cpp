//cpp
// func_ov002_020bf9d4 at 0x020bf9d4
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
struct VecFx32 { int x, y, z; };
extern "C" {
int func_0200fccc(char* s, int r1);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
extern int data_0208e430;
}
extern "C" void func_ov002_020bf9d4(char* c){
  VecFx32 v;
  if (data_0208e430 == 0x2e) return;
  if (*(unsigned char*)(c + 0x703) == 0){
    func_0200fccc(c, 0);
    return;
  }
  ((int*)&v)[0] = *(int*)(c + 0x5c);
  ((int*)&v)[1] = *(int*)(c + 0x60);
  ((int*)&v)[2] = *(int*)(c + 0x64);
  if (*(unsigned char*)(c + 0x707) != 0){
    v.y += 0x19000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x5c, v.x, v.y, v.z);
  } else {
    v.y += 0x23000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x5b, v.x, v.y, v.z);
  }
}