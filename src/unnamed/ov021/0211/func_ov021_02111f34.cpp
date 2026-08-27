//cpp
extern "C" int func_ov021_02111f1c(char *b, char *c);
extern "C" int func_ov021_02111f34(int a, char *b, char *c){
  int ok = (*(unsigned short*)(c+0xc)==0xbf);
  if(ok==0) return ok;
  return func_ov021_02111f1c(b, c);
}
