extern int DecIfAbove0_Byte(void* p);
extern void* _ZN8dActor_c13ClosestPlayerEv(void* a);
extern int Vec3_Dist(void* x, void* y);
extern int _ZN6Player9StartTalkER7fBase_cb(void* pl, void* a, int b);
extern int func_ov019_021122dc(char* t, int v);
int func_ov019_02112168(char* c){
  if(DecIfAbove0_Byte((char*)c+0x38e)) return 1;
  void* pl = _ZN8dActor_c13ClosestPlayerEv(c);
  if(pl == 0) return 1;
  int d178 = *(int*)((char*)c+0x178);
  if(Vec3_Dist((char*)c+0x5c, (char*)pl+0x5c) < d178 + 0x78000){
    if(_ZN6Player9StartTalkER7fBase_cb(pl, c, 1)){
      *(int*)((char*)c+0x378) = (int)pl;
      func_ov019_021122dc(c, 1);
    }
  }
  return 1;
}
