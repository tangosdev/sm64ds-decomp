//cpp
extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void* c, void* st);
extern int _ZN6Player7ST_HURTE[];
int _ZN6Player12GetHurtStateEv(char* c){
  if(_ZN6Player7IsStateERNS_5StateE(c, _ZN6Player7ST_HURTE))
    return *(unsigned char*)(c+0x6e3) & 7;
  return -1;
}
}
