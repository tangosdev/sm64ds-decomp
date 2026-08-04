//cpp
extern "C" {
struct Ent{ int a; int b; };
extern Ent data_ov006_02141f44[];
void func_ov006_020e82fc(char* c){
  unsigned char idx = *(unsigned char*)(c+0x5000+0x552);
  if(idx >= 3) return;
  Ent* e=&data_ov006_02141f44[idx];
  int adj=e->b;
  char* obj=c+(adj>>1);
  int fn;
  if(adj&1){
    fn=*(int*)(*(int*)obj + e->a);
  } else {
    fn=e->a;
  }
  ((void(*)(void*))fn)(obj);
}
}
