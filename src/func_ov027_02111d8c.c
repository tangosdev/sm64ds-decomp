extern void _ZN13SharedFilePtr7ReleaseEv(void*);
extern int _ZN16MeshColliderBase9IsEnabledEv(void*);
extern void _ZN16MeshColliderBase7DisableEv(void*);
extern void* data_ov027_02113c7c;
extern void* data_ov035_02112ca4[];
extern void* data_ov027_02113c94;
extern void* data_ov027_02113c6c;
int func_ov027_02111d8c(char* c){
  int i;
  _ZN13SharedFilePtr7ReleaseEv(&data_ov027_02113c7c);
  for(i=0;i<3;i++) _ZN13SharedFilePtr7ReleaseEv(data_ov035_02112ca4[i]);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov027_02113c94);
  if(_ZN16MeshColliderBase9IsEnabledEv(c+0x124))
    _ZN16MeshColliderBase7DisableEv(c+0x124);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov027_02113c6c);
  return 1;
}
