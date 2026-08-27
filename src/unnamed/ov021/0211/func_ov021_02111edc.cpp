//cpp
extern "C" {
int func_ov021_02111ec4(void *p, void *q);
int func_ov021_02111edc(void *a, void *b, char *c){
  bool x = (*(unsigned short*)((char*)c+0xc) == 0xbf);
  if(!x) return x;
  return func_ov021_02111ec4(b, c);
}
}
