//cpp
extern "C" {
extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN6Player4BurnEv(void* p);
void func_ov060_02116740(char* c){
  unsigned int id = *(unsigned int*)(c+0x2f4);
  if(id==0) return;
  char* p = _ZN5Actor10FindWithIDEj(id);
  if(p==0) return;
  int b = (int)(*(unsigned short*)(p+0xc) == 0xbf);
  if(b==0) return;
  if(*(unsigned char*)(p+0x6f9)!=0) return;
  if(*(unsigned char*)(p+0x6fb)!=0) return;
  _ZN6Player4BurnEv(p);
}
}
