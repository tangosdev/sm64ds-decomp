//cpp
extern "C" {
typedef struct Heap Heap;
void* func_ov004_020b929c(void* self);
void _ZN9ActorBaseD1Ev(void* t);
void _ZN6Memory10DeallocateEPvP4Heap(void* p, Heap* h);
extern void* data_ov004_020bc0c0;
extern void* data_ov004_020beb68;
extern void* _ZTV5Stage;
extern void* data_0208e4b8;
extern Heap* data_020a0eac;
void* func_ov004_020b2a18(void* self){
  char* t=(char*)self;
  *(void**)t = &data_ov004_020bc0c0;
  *(void**)&data_ov004_020beb68 = 0;
  func_ov004_020b929c(t+0xf4);
  *(void**)t = &_ZTV5Stage;
  *(void**)t = &data_0208e4b8;
  _ZN9ActorBaseD1Ev(t);
  _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
  return t;
}
}
