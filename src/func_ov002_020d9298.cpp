//cpp
extern "C" {
struct State;
struct Vector3;
extern State _ZN6Player15ST_GROUND_POUNDE;
extern State _ZN6Player12ST_BURN_LAVAE;
extern State _ZN6Player7ST_JUMPE;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, Vector3* v);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* c, State* st);
int func_ov002_020d9298(char* c){
  if (_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player15ST_GROUND_POUNDE)) return 0;
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char*)(c+0x6d9), 0x14, (Vector3*)(c+0x74));
  if (_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player12ST_BURN_LAVAE)) { *(int*)(c+0xa8)=0x40000; return 1; }
  if (_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player7ST_JUMPE)) { *(short*)(c+0x6a8)=0xa; }
  _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_JUMPE);
  return 1;
}
}
