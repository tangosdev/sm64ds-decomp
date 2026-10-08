//cpp
extern "C" {
void func_ov025_021125bc(char* a, char* b){
  long eq = (*(unsigned short*)(b+0xc)==0xbf);
  eq ? (*(bool*)(a+0x3f7)=true) : false;
}
}
