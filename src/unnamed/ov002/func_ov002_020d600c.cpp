//cpp
extern "C" {
struct State;
extern State data_ov002_02110064;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
int func_ov002_020d600c(char* c){
  if(_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_02110064)){
    if(*(unsigned char*)(c+0x6e3)==1) return 1;
  }
  return 0;
}
}
