extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern int data_ov098_0213c4c8[];
int func_ov098_02139a50(char *t){
  int* f;
  if(_ZN16MeshColliderBase9IsEnabledEv(t+0x124))
    _ZN16MeshColliderBase7DisableEv(t+0x124);
  f = 0;
  if(*(unsigned short*)(t+0xc) == 0xc2) f = data_ov098_0213c4c8;
  if(f){
    _ZN13SharedFilePtr7ReleaseEv((void*)f[0]);
    _ZN13SharedFilePtr7ReleaseEv((void*)f[1]);
  }
  return 1;
}
