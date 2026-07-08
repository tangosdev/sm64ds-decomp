//cpp
extern "C" {
struct State;
extern State _ZN6Player7ST_TALKE;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
int _ZN6Player12GetTalkStateEv(char* c){
  if(!_ZN6Player7IsStateERNS_5StateE(c,&_ZN6Player7ST_TALKE)) return -1;
  unsigned char v=*(unsigned char*)(c+0x6e3);
  if(v==0) return 0;
  if(v==3) return 2;
  if(v==5||v==7) return 3;
  return 1;
}
}
