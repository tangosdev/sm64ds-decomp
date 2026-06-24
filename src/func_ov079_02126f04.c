extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern void* data_ov079_02127f64[];
extern void* data_ov079_02128300;
extern void* data_ov079_021282f0;
int func_ov079_02126f04(char *t){
  if(_ZN16MeshColliderBase9IsEnabledEv(t+0x124))
    _ZN16MeshColliderBase7DisableEv(t+0x124);
  _ZN13SharedFilePtr7ReleaseEv(data_ov079_02127f64[0]);
  _ZN13SharedFilePtr7ReleaseEv(data_ov079_02127f64[1]);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov079_02128300);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov079_021282f0);
  return 1;
}
