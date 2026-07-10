extern int _ZN16MeshColliderBase9IsEnabledEv(void *);
extern void _ZN16MeshColliderBase7DisableEv(void *);
extern void _ZN13SharedFilePtr7ReleaseEv(void *);
struct SFP { void *a, *b, *c; };
extern struct SFP data_ov091_02134c30[];
extern struct SFP data_ov091_02134c34[];
int _ZN25RotatingUpDownPlatformUtm16CleanupResourcesEv(char *t){
  if(*(int*)(t+8) == 0xffff) return 1;
  if(_ZN16MeshColliderBase9IsEnabledEv(t+0x124))
    _ZN16MeshColliderBase7DisableEv(t+0x124);
  _ZN13SharedFilePtr7ReleaseEv(data_ov091_02134c30[*(unsigned char*)(t+0x395)].a);
  _ZN13SharedFilePtr7ReleaseEv(data_ov091_02134c34[*(unsigned char*)(t+0x395)].a);
  return 1;
}
