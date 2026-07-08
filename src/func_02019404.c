extern void* SCENE_RELATED;
void func_02019404(void){
  void** o = (void**)SCENE_RELATED;
  if(!o) return;
  void** vt = (void**)o[0];
  ((void(*)(void*))vt[1])(o);
}
