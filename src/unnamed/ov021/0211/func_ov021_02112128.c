extern int func_ov021_02112024(void*,void*);
int func_ov021_02112128(int a, void* b, void* c2){
  int t = *(unsigned short*)((char*)c2+0xc)==0xbf;
  if(t){ return func_ov021_02112024(b,c2); }
  return t;
}
