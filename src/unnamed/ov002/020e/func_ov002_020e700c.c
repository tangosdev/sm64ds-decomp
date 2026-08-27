extern void* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int, void*);
extern int Vec3_Dist(void*, void*);
void func_ov002_020e700c(void* c) {
  void* o;
  if (*(unsigned char*)((char*)c+0x4a0)) return;
  o = _ZN8dActor_c15FindWithActorIDEjPS_(0x12, 0);
  while (o) {
    if (Vec3_Dist((char*)c+0x5c, (char*)o+0x5c) < 0xc8000) {
      *(unsigned char*)((char*)c+0x49f) = 1;
      *(void**)((char*)o+0x364) = c;
      break;
    }
    o = _ZN8dActor_c15FindWithActorIDEjPS_(0x12, o);
  }
  *(unsigned char*)((char*)c+0x4a0) = 1;
}
