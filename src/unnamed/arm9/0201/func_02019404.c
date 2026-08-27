extern void* data_0209d4a8;
void func_02019404(void){
  void** o = (void**)data_0209d4a8;
  if(!o) return;
  void** vt = (void**)o[0];
  ((void(*)(void*))vt[1])(o);
}
