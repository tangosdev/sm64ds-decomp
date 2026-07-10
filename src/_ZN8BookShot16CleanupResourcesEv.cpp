//cpp
extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
void UnloadBlueCoinModel(void *);
extern int data_ov020_02114aa0;
extern int data_ov020_02114ab8;
extern int data_ov020_02114aa8;
extern int data_ov020_02114ab0;
int _ZN8BookShot16CleanupResourcesEv(void *c){
  _ZN13SharedFilePtr7ReleaseEv(&data_ov020_02114aa0);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov020_02114ab8);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov020_02114aa8);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov020_02114ab0);
  UnloadBlueCoinModel(c);
  return 1;
}
}
