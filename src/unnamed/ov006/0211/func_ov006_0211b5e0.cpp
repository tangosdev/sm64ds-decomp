//cpp
extern "C" {
struct Ent{ int a; int b; };
extern Ent data_ov006_02142e20[];
void func_ov006_0211b5e0(char* c){
  int i=0;
  char* r5=c;
  do{
    if(*(unsigned char*)(r5+0x51cc)!=0){
      int idx=*(unsigned char*)(r5+0x51d0);
      Ent* e=&data_ov006_02142e20[idx];
      int adj=e->b;
      char* obj=c+(adj>>1);
      int fn;
      if(adj&1){
        fn=*(int*)(*(int*)obj + e->a);
      } else {
        fn=e->a;
      }
      ((void(*)(void*,int))fn)(obj,i);
    }
    i++;
    r5+=0x24;
  }while(i<30);
}
}
