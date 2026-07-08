extern void func_ov002_020bdef0(void*);
extern void func_ov002_020bdd9c(void*);
extern void func_ov002_020d80d0(void*);
extern int _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern void* _ZN6Player8ST_SHELLE;
int func_ov002_020cc16c(void* c, void* state) {
  if (*(int*)((char*)c+0x354) != 0) return 0;
  if (*(unsigned char*)((char*)c+0x6f9) != 0 || *(unsigned char*)((char*)c+0x6fb) != 0) {
    func_ov002_020bdef0(c);
    func_ov002_020bdd9c(c);
  }
  if (*(unsigned char*)((char*)c+0x6f8) != 0) func_ov002_020d80d0(c);
  *(void**)((char*)c+0x354) = state;
  _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player8ST_SHELLE);
  return 1;
}
