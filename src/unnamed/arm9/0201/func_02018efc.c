extern void* data_0209d4ac;
void func_02018efc(void){
  void** o = (void**)data_0209d4ac;
  if(!o) return;
  void** vt = (void**)o[0];
  ((void(*)(void*))vt[2])(o);
}
