//cpp
extern "C" {
typedef unsigned int u32;
struct SharedFilePtr { u32 data[4]; };
void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);
extern struct SharedFilePtr data_ov019_02113498;
extern struct SharedFilePtr* _ZN9HugeCover6RenderEv[7];
extern struct SharedFilePtr* data_ov019_0211277c[3];
int _ZN13RacingPenguin16CleanupResourcesEv(void){
  int i;
  _ZN13SharedFilePtr7ReleaseEv(&data_ov019_02113498);
  for(i=0;i<7;i++) _ZN13SharedFilePtr7ReleaseEv(_ZN9HugeCover6RenderEv[i]);
  for(i=0;i<3;i++) _ZN13SharedFilePtr7ReleaseEv(data_ov019_0211277c[i]);
  return 1;
}
}
