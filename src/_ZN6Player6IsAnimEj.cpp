//cpp
extern "C" unsigned int _ZNK6Player14GetBodyModelIDEjb(char*,unsigned int,char);
extern int _ZN6Player9ANIM_PTRSE[];
extern "C" int _ZN6Player6IsAnimEj(char* c, unsigned int a){
  int ip = *(int*)(c+8);
  int* p = (int*)_ZN6Player9ANIM_PTRSE[ip + a*4];
  int v = p[1];
  unsigned int id = _ZNK6Player14GetBodyModelIDEjb(c, ip&0xff, 0);
  int* m = *(int**)(c + id*4 + 0xdc);
  int r=(m[24]==v); return r?1:0;
}
