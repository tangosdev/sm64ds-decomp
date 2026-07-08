//cpp
extern "C" int _ZN6Player7IsStateERNS_5StateE(char*, void*);
extern int _ZN6Player11ST_DEAD_HITE[];
extern int _ZN6Player11ST_DEAD_PITE[];
extern "C" int GiveHealth(int,int);
extern "C" int _ZN6Player4HealEi(char* c, int amt){
  int r = _ZN6Player7IsStateERNS_5StateE(c, _ZN6Player11ST_DEAD_HITE);
  if(r) return r;
  r = _ZN6Player7IsStateERNS_5StateE(c, _ZN6Player11ST_DEAD_PITE);
  if(r) return r;
  return GiveHealth(*(unsigned char*)(c+0x6d8), amt);
}
