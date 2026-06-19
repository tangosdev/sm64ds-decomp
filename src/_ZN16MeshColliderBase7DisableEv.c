extern int func_02039404(unsigned char *p);
extern void func_020393f0(char *p);
extern int data_020a0c80[];
int _ZN16MeshColliderBase7DisableEv(void *c){
  int i = func_02039404((unsigned char*)c);
  if (i != 0x18) {
    func_020393f0((char*)c);
    data_020a0c80[i] = 0;
  }
  return 1;
}
