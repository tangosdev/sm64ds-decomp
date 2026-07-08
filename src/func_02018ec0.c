extern void* KS_FADER;
void func_02018ec0(void){
  void** o = (void**)KS_FADER;
  if(!o) return;
  void** vt = (void**)o[0];
  ((void(*)(void*))vt[2])(o);
}
