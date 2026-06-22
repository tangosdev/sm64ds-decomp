//cpp
extern "C" {
extern int func_ov006_02104ac4(void*);
extern int func_ov006_02104354(void*);
struct Ent{ int a; int b; };
extern Ent data_ov006_02142888[];
int func_ov006_02107358(char* c){
  int idx=*(int*)(c+0x4000+0xca8);
  Ent* e=&data_ov006_02142888[idx];
  int adj=e->b;
  char* obj=c+(adj>>1);
  int fn;
  if(adj&1){
    fn=*(int*)(*(int*)obj + e->a);
  } else {
    fn=e->a;
  }
  ((void(*)(void*))fn)(obj);
  func_ov006_02104ac4(c);
  func_ov006_02104354(c);
  return 1;
}
}
