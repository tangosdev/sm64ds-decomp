//cpp
typedef struct { int x, y, z; } Vec;
extern "C" {
char* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int, void*);
void _ZN7fBase_c18MarkForDestructionEv(void*);
void func_ov064_02118da0(char* c){
  volatile int pad[4];
  char* a = _ZN8dActor_c15FindWithActorIDEjPS_(0x4f, 0);
  (void)&pad;
  if(a!=0){
    Vec* p = (Vec*)(((int)a + 0x5c));
    *(int*)(c+0x320) = *(int*)(a+4);
    int z = p->z;
    int x = p->x;
    int y = *(int*)(c+0x60);
    *(int*)(a+0x5c) = x;
    *(int*)(a+0x60) = y;
    *(int*)(a+0x64) = z;
    *(unsigned char*)(c+0x339) = 0;
    *(unsigned char*)(((int)c + 0x336)) = *(unsigned char*)(((int)c + 0x336)) + 1;
    return;
  }
  _ZN7fBase_c18MarkForDestructionEv(c);
}
}
