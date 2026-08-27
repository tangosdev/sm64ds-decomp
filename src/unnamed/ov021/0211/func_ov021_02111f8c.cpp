//cpp
extern "C" {
int func_ov021_02111f74(void* a, void* b);
int func_ov021_02111f8c(void* a, void* b, char* c){
  bool x = (*(unsigned short*)(c+0xc)==0xbf);
  if(!x) return x;
  return func_ov021_02111f74(b, c);
}
}
