//cpp
extern "C" {
struct State;
extern State _ZN6Player7ST_TALKE;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
int _ZN6Player12Unk_020c4f40Et(char* c, unsigned short x){
  if(_ZN6Player7IsStateERNS_5StateE(c,&_ZN6Player7ST_TALKE) && *(unsigned char*)(c+0x6e3)==3){
    *(unsigned short*)(c+0x6a6)=x;
    *(unsigned char*)(c+0x6e3)=5;
    return 1;
  }
  return 0;
}
}
