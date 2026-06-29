//cpp
// _ZN6Player13St_Throw_InitEv at 0x020dafd4
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
extern "C" {
struct Vector3{int x,y,z;};
extern int _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned a, unsigned b, const Vector3& v);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*,unsigned,int,int,unsigned);
int _ZN6Player13St_Throw_InitEv(char* c){
  int* p = *(int**)(c+0x358);
  int b = (*(int*)((char*)p+0xb0) & 0x200) ? 1 : 0;
  if(!b){
    unsigned short a = *(unsigned short*)((char*)p+0xc);
    int t1 = (a == 0xbd);
    if(!t1){
      int t2 = (a == 0xbe);
      if(!t2) goto anim30;
    }
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x91,0x40000000,0x1000,0);
    goto voice;
anim30:
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x30,0x40000000,0x1000,0);
voice:
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 0x13, *(Vector3*)(c+0x74));
  } else {
    _ZN6Player7SetAnimEji5Fix12IiEj(c,0x8a,0x40000000,0x1000,0);
  }
  return 1;
}
}
