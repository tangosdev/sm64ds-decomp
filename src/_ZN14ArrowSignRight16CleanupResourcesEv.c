extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern char data_ov098_0213c380[];
extern char data_ov098_0213c384[];
int _ZN14ArrowSignRight16CleanupResourcesEv(char *t){
  if(_ZN16MeshColliderBase9IsEnabledEv(t+0x124))
    _ZN16MeshColliderBase7DisableEv(t+0x124);
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov098_0213c380 + (unsigned char)t[0x37c]*0xc));
  _ZN13SharedFilePtr7ReleaseEv(*(void**)(data_ov098_0213c384 + (unsigned char)t[0x37c]*0xc));
  return 1;
}
