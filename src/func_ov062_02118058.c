typedef int Fix12i;
extern Fix12i Vec3_Dist(const void*, const void*);
extern int _ZN5Actor14GetSubtractionEss(void*, short, short);
extern int func_ov065_02117994(void*, int);
int func_ov062_02118058(char *c){
  void *o=*(void**)(c+0x3b4);
  int d=*(int*)(c+0x3b8);
  if(o!=0) d=Vec3_Dist(c+0x5c, (char*)o+0x5c);
  if(d>=0x12c000) return d;
  int s=_ZN5Actor14GetSubtractionEss(c, *(short*)(c+0x3c0), *(short*)(c+0x94));
  if(s>=0x3000) return s;
  if(*(int*)(c+0x390)==1) *(int*)(c+0x38c)=1;
  else *(int*)(c+0x38c)=3;
  return func_ov065_02117994(c, 3);
}
