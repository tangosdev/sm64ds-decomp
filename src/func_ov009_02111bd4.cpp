//cpp
// @symbol func_ov009_02111bd4
// @emits daObjMcWater_c_CleanupResources
/* recovered: renamed to Class_Method */
/* daObjMcWater_c::CleanupResources - recovered from vtable slot identity */
extern "C" {
int _ZN13SharedFilePtr7ReleaseEv(void*);
int _ZN16MeshColliderBase9IsEnabledEv(void*);
int _ZN16MeshColliderBase7DisableEv(void*);
extern int data_ov009_02113c68[];
extern int data_ov009_02113c70[];
int daObjMcWater_c_CleanupResources(char* c){
  _ZN13SharedFilePtr7ReleaseEv((void*)data_ov009_02113c68);
  _ZN13SharedFilePtr7ReleaseEv((void*)data_ov009_02113c70);
  if(_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
    _ZN16MeshColliderBase7DisableEv(c+0x124);
  return 1;
}
}
