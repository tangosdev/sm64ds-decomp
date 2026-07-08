extern void _ZN18TextureTransformerD1Ev(void*);
extern void func_ov006_020e80d8(void*);
extern void _ZN5ModelD1Ev(void*);
extern void _ZN8Particle10SysTrackerD1Ev(void*);
extern void func_ov004_020b29c0(void*);
extern void _ZN6Memory10DeallocateEPvP4Heap(void*, void*);
extern int data_ov006_0213c8c4;
extern int data_ov006_0213e448;
extern void* _ZN6Memory11gameHeapPtrE;
void* func_ov006_020e76e4(char* p){
  *(int*)p = (int)&data_ov006_0213c8c4;
  _ZN18TextureTransformerD1Ev(p + 0x51f4);
  func_ov006_020e80d8(p + 0x4fd8);
  _ZN5ModelD1Ev(p + 0x4f88);
  _ZN5ModelD1Ev(p + 0x4f38);
  *(int*)p = (int)&data_ov006_0213e448;
  _ZN8Particle10SysTrackerD1Ev(p + 0x471c);
  func_ov004_020b29c0(p);
  _ZN6Memory10DeallocateEPvP4Heap(p, _ZN6Memory11gameHeapPtrE);
  return p;
}
