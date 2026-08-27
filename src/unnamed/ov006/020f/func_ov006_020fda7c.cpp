//cpp
extern "C" {
struct Ent{ int a; int b; };
extern Ent data_ov006_02142694[];
void func_ov006_020fda7c(char* c){
  int i=0;
  char* r5=c;
  do{
    if(*(unsigned char*)(r5+0x468c)!=0){
      int idx=*(unsigned char*)(r5+0x468f);
      Ent* e=&data_ov006_02142694[idx];
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
    r5+=0x38;
  }while(i<30);
}
}
