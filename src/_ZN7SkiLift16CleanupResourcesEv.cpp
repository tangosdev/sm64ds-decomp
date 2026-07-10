//cpp
extern "C" {
typedef unsigned int u32;
struct SharedFilePtr { u32 data[4]; };
void _ZN13SharedFilePtr7ReleaseEv(struct SharedFilePtr *self);
extern struct SharedFilePtr data_ov036_02113c00;
extern struct SharedFilePtr* data_ov018_02112c0c[2];
extern struct SharedFilePtr* data_ov056_02112c04[2];
int _ZN7SkiLift16CleanupResourcesEv(void){
  int i;
  _ZN13SharedFilePtr7ReleaseEv(&data_ov036_02113c00);
  for(i=0;i<2;i++) _ZN13SharedFilePtr7ReleaseEv(data_ov018_02112c0c[i]);
  for(i=0;i<2;i++) _ZN13SharedFilePtr7ReleaseEv(data_ov056_02112c04[i]);
  return 1;
}
}
