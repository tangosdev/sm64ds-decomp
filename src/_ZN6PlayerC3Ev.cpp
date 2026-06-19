//cpp
extern "C" {
void* _ZN9ActorBasenwEj(unsigned int);
void* _ZN6PlayerC1Ev(void*);
void* _ZN6PlayerC3Ev(){
  void* r=_ZN9ActorBasenwEj(0x768);
  if(r==0) return r;
  return _ZN6PlayerC1Ev(r);
}
}
