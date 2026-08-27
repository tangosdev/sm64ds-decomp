//cpp
extern "C" {
void func_ov095_02136764(char* a, char* b){
  long eq = (*(unsigned short*)(b+0xc)==0xbf);
  if(eq){ *(char**)(a+0x320)=b; *(unsigned char*)(a+0x348)=1; }
}
}
