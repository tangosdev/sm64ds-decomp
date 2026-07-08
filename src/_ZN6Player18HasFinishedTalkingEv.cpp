//cpp
extern "C" {
struct State;
extern State _ZN6Player7ST_TALKE;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
int _ZN6Player18HasFinishedTalkingEv(char* c){
  if(_ZN6Player7IsStateERNS_5StateE(c,&_ZN6Player7ST_TALKE) && *(unsigned char*)(c+0x6e3)==3){
    *(unsigned char*)(c+0x6e3)=4;
    return 1;
  }
  return 0;
}
}
