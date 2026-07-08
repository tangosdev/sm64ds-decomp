extern int _ZN6Player7IsStateERNS_5StateE(void* thiz, void* state);
extern void _ZN6Player14St_Owl_CleanupEv(void* thiz);
extern void _ZN6Player11ChangeStateERNS_5StateE(void* thiz, void* state);
extern char data_ov002_02110244[];
extern char _ZN6Player7ST_FALLE[];
int func_ov002_020df7ac(void* thiz){
  if (_ZN6Player7IsStateERNS_5StateE(thiz, data_ov002_02110244) == 0) return 0;
  _ZN6Player14St_Owl_CleanupEv(thiz);
  _ZN6Player11ChangeStateERNS_5StateE(thiz, _ZN6Player7ST_FALLE);
  return 1;
}
