extern void* data_0209f1e4;
void func_0202345c(void){
  void** o = (void**)data_0209f1e4;
  if(!o) return;
  void** vt = (void**)o[0];
  ((void(*)(void*))vt[2])(o);
}
