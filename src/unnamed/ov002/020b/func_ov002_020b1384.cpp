//cpp
extern "C" {
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void*);
extern int _ZNK10dBgCh_Actr13GetWallResultEv(void*);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void*, void*);
extern short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void*, int, int, short);
void func_ov002_020b1384(char* c){
  if(_ZNK10dBgCh_Actr8IsOnWallEv(c+0x1ac)==0) return;
  int v[3];
  void* w=(void*)_ZNK10dBgCh_Actr13GetWallResultEv(c+0x1ac);
  _ZNK11SurfaceInfo12CopyNormalToER7Vector3((char*)w+4, v);
  *(short*)(c+0x94)=_ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(c, v[0], v[2], *(short*)(c+0x94));
}
}
