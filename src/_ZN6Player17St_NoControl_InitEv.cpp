//cpp
extern "C" {
extern int func_ov002_020c9e40(void* c);
extern int func_ov002_020dab14(void* c);
struct PMF { int adj; int ptr; };
extern struct PMF data_ov002_02110884[];
int _ZN6Player17St_NoControl_InitEv(char* c){
  *(unsigned char*)(c+0x6f6)=1;
  *(unsigned char*)(c+0x6e6)=0;
  func_ov002_020c9e40(c);
  func_ov002_020dab14(c);
  *(int*)(c+0x354)=0;
  *(int*)(c+0x358)=0;
  *(int*)(c+0x35c)=0;
  *(unsigned char*)(c+0x70b)=0;
  unsigned idx = *(unsigned char*)(c+0x70a);
  struct PMF* m=&data_ov002_02110884[idx];
  char* obj=c + (m->ptr >> 1);
  int fn=m->ptr;
  void (*f)(void*);
  if(fn & 1){
    f=*(void(**)(void*))(*(int*)obj + m->adj);
  } else {
    f=(void(*)(void*))m->adj;
  }
  f(obj);
  return 1;
}
}
