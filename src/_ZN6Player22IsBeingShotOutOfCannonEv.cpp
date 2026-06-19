//cpp
extern "C" {
struct State;
extern State data_ov002_021102d4;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
int _ZN6Player22IsBeingShotOutOfCannonEv(char* c){
  if(!_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021102d4)) return 0;
  return *(unsigned char*)(c+0x6e3)==2 ? 1 : 0;
}
}
