//cpp
extern "C" {
struct Ent{ int a; int b; };
extern Ent data_ov006_02142df8[];
void func_ov006_0211b954(char* c){
  int i=0;
  char* r5=c;
  do{
    if(*(unsigned char*)(r5+0x50f5)!=0){
      int idx=*(unsigned char*)(r5+0x50f4);
      Ent* e=&data_ov006_02142df8[idx];
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
    r5+=0x14;
  }while(i<10);
}
}
