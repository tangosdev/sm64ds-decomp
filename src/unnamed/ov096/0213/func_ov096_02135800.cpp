//cpp
extern "C" void func_ov096_02135800(char* c){
  int b = (*(unsigned short*)(c+0xc) == 0xf0);
  if(b) return;
  char* next = *(char**)(c+0x394);
  char* prev = *(char**)(c+0x390);
  *(char**)(prev+0x394) = next;
  next = *(char**)(c+0x394);
  if(next) *(char**)(next+0x390) = *(char**)(c+0x390);
}
