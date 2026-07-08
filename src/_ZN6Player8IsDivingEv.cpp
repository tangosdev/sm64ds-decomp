//cpp
extern "C" {
extern int _ZN6Player7ST_DIVEE[];
int _ZN6Player7IsStateERNS_5StateE(void*, void*);
int _ZN6Player8IsDivingEv(void* c){
  return _ZN6Player7IsStateERNS_5StateE(c, _ZN6Player7ST_DIVEE) != 0;
}
}
