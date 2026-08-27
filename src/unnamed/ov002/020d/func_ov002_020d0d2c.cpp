//cpp
extern "C" {
struct State;
extern State data_ov002_0210ffec;
extern State data_ov002_02110004;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
int func_ov002_020d0d2c(void* c){
  if(_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_0210ffec) || _ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110004)) return 1;
  return 0;
}
}
