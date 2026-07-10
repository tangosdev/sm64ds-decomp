//cpp
extern "C" void _ZN16MeshColliderBase7DisableEv(char*);
extern "C" void _ZN13SharedFilePtr7ReleaseEv(void*);
extern char func_ov030_02113be8[];
extern char data_ov027_02113be0[];
extern "C" int _ZN10SlidingIce16CleanupResourcesEv(char *c){
  unsigned char ok = (*(unsigned short*)(c+0xc)==0x5d);
  if(ok){ _ZN16MeshColliderBase7DisableEv(c+0x124); }
  _ZN13SharedFilePtr7ReleaseEv(func_ov030_02113be8);
  _ZN13SharedFilePtr7ReleaseEv(data_ov027_02113be0);
  return 1;
}
