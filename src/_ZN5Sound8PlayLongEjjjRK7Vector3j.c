struct Vector3 { int x,y,z; };
extern int func_02048a1c(struct Vector3* v);
extern void* func_02011934(int* table, int id);
extern void func_020123c8(char* c, unsigned int a, unsigned int b, struct Vector3* v);
extern int func_0201179c(int* g, unsigned int a, unsigned int b, struct Vector3* v, short x);
extern int data_0209b53c[];
int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int j1, unsigned int j2, unsigned int j3, struct Vector3* v, short j5){
  if(func_02048a1c(v)==0) return 0;
  char* p = (char*)func_02011934(data_0209b53c, j1);
  if(p){
    *(char*)(p+6)=1;
    func_020123c8(p+8, j2, j3, v);
    return *(int*)p;
  }
  return func_0201179c(data_0209b53c, j2, j3, v, j5);
}
