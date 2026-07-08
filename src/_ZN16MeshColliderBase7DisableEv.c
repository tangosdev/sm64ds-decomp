extern int func_02039404(unsigned char *p);
extern void func_020393f0(char *p);
extern int ACTIVE_MESH_COLLIDERS[];
int _ZN16MeshColliderBase7DisableEv(void *c){
  int i = func_02039404((unsigned char*)c);
  if (i != 0x18) {
    func_020393f0((char*)c);
    ACTIVE_MESH_COLLIDERS[i] = 0;
  }
  return 1;
}
