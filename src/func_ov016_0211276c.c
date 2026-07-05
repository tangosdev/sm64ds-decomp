extern int _ZN16MeshColliderBase9IsEnabledEv(void* m);
extern int _ZN16MeshColliderBase6EnableEP5Actor(void* m, void* a);
extern void func_020393a4(int* p, int v);
extern int _ZN5Actor13DistToCPlayerEv(void* a);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int cc, void* v, unsigned int e);
extern void func_ov016_021126a8(char* t);
extern int _ZN8Platform19UpdateClsnPosAndRotEv(void* a);
extern short data_02082214[];
int func_ov016_0211276c(char* c){
  if(_ZN16MeshColliderBase9IsEnabledEv(c+0x124) == 0){
    _ZN16MeshColliderBase6EnableEP5Actor(c+0x124, c);
  }
  func_020393a4((int*)(c+0x124), 0x2000000);
  if(*(unsigned char*)(c+0x31e) == 0){
    *(short*)(((int)c + 0x320) & 0xFFFFFFFFFFFFFFFF) += 0xda;
    *(short*)(c+0x8c) = (short)((*(short*)((char*)data_02082214 + ((*(unsigned short*)(c+0x320)>>4)<<2)) << 0xa) >> 0xc);
    if(_ZN5Actor13DistToCPlayerEv(c) < 0xbb8000){
      *(int*)(c+0x324) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int*)(c+0x324), 3, 0x8b, c+0x74, 0);
    }
    func_ov016_021126a8(c);
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
  }
  return 1;
}
