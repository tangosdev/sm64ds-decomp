//cpp
extern "C" {
struct Ent{ int a; int b; };
extern Ent data_ov006_02142624[];
void func_ov006_020fc7d0(char* c){
  int i=0;
  char* r5=c;
  do{
    if(*(unsigned char*)(r5+0x4000+0xeb7)!=0){
      int idx=*(unsigned char*)(r5+0x4000+0xeb3);
      Ent* e=&data_ov006_02142624[idx];
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
    r5+=0x1c;
  }while(i<2);
}
}
