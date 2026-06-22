extern int func_ov006_0210dbb0(int a, void* p);
struct V2 { int x; int y; };
int func_ov006_02111e90(char* c, int* src){
  int g = *(int*)(*(int*)(c+4)+0x4000+0x780);
  if(g == 0) return 0;
  struct V2 v;
  v.x = src[0];
  v.y = src[1];
  return func_ov006_0210dbb0(*(int*)(*(int*)(c+4)+0x4000+0x780), &v);
}
