typedef int Fix12i;
typedef struct Vector3 { int x, y, z; } Vector3;
extern void *_ZN8dActor_c15FindWithActorIDEjPS_(unsigned int id, void *start);
extern Fix12i Vec3_Dist(const Vector3 *a, const Vector3 *b);
void func_ov002_020b1008(char *c){
  char* actor;
  if (*(unsigned char*)(c+0x3af)) return;
  if (*(int*)(c+0x3a0) != 1) return;
  actor = (char*)_ZN8dActor_c15FindWithActorIDEjPS_(0xf, 0);
  while (actor) {
    if (Vec3_Dist((Vector3*)(c+0x5c), (Vector3*)(actor+0x5c)) < 0x96000) {
      *(unsigned char*)(c+0x3b0) = 1;
      *(void**)(actor+0x328) = c;
      break;
    }
    actor = (char*)_ZN8dActor_c15FindWithActorIDEjPS_(0xf, actor);
  }
  *(unsigned char*)(c+0x3af) = 1;
}
