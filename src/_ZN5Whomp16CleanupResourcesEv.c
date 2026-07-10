extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
extern void *data_ov079_02127bf0[];
extern void data_ov079_02128168(void);
extern void *data_ov079_02127600[];
extern void data_ov079_02128178(void);
extern void data_ov079_02128170(void);
extern void *data_ov079_021275ec[];
int _ZN5Whomp16CleanupResourcesEv(unsigned char *t){
  int i;
  if(_ZN16MeshColliderBase9IsEnabledEv(t+0x418))
    _ZN16MeshColliderBase7DisableEv(t+0x418);
  _ZN13SharedFilePtr7ReleaseEv(data_ov079_02127bf0[t[0x414]]);
  if(t[0x414]){
    _ZN13SharedFilePtr7ReleaseEv((void*)&data_ov079_02128168);
    for(i=0;i<6;i++)
      _ZN13SharedFilePtr7ReleaseEv(data_ov079_02127600[i]);
    _ZN13SharedFilePtr7ReleaseEv((void*)&data_ov079_02128178);
  } else {
    _ZN13SharedFilePtr7ReleaseEv((void*)&data_ov079_02128170);
    for(i=0;i<5;i++)
      _ZN13SharedFilePtr7ReleaseEv(data_ov079_021275ec[i]);
  }
  return 1;
}
