// @symbol func_ov052_0211123c
// recovered name: daObjEmmLog_c_CleanupResources
/* recovered: renamed to Class_Method */
/* daObjEmmLog_c::CleanupResources - name recovered from the vtable slot it fills.
   The body is a decompilation verified against the ROM, not an
   inferred stub. */
extern int _ZN16MeshColliderBase9IsEnabledEv();
extern int _ZN16MeshColliderBase7DisableEv();
extern int _ZN13SharedFilePtr7ReleaseEv();
extern int *data_ov052_021124d4[];
int func_ov052_0211123c(char *c){
  if(_ZN16MeshColliderBase9IsEnabledEv((char*)c+0x124))
    _ZN16MeshColliderBase7DisableEv((char*)c+0x124);
  _ZN13SharedFilePtr7ReleaseEv(data_ov052_021124d4[0]);
  _ZN13SharedFilePtr7ReleaseEv(data_ov052_021124d4[1]);
  return 1;
}
