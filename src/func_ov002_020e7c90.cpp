//cpp
extern "C" {
extern char* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, char* prev);
extern void _ZN6Camera9SetLookAtERK7Vector3(void* cam, void* v);
extern void _ZN6Camera6SetPosERK7Vector3(void* cam, void* v);
int func_ov002_020e7c90(char* c, void* cam){
  char* a = 0;
  for(;;){
    a = _ZN8dActor_c15FindWithActorIDEjPS_(0xb1, a);
    if(a == 0) break;
    if(*(unsigned char*)(c+0x49d) == (*(unsigned int*)(a+8) & 0xf)){
      _ZN6Camera9SetLookAtERK7Vector3(cam, c+0x5c);
      _ZN6Camera6SetPosERK7Vector3(cam, a+0x5c);
      return 1;
    }
  }
  return 0;
}
}
