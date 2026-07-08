//cpp
extern "C" {
struct State { int a, b, c; };
extern int _ZN6Player7IsStateERNS_5StateE(void*, State*);
extern State _ZN6Player7ST_TALKE;
int func_ov002_020c51d0(char* c, State* st){
  if (!_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player7ST_TALKE)) return 0;
  if (*(unsigned char*)(c+0x6e3) != 0) return 0;
  *(State*)(c+0x744) = *st;
  *(unsigned char*)(c+0x6e5) = 1;
  return 1;
}
}
