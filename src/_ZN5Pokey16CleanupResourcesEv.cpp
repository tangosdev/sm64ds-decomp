//cpp
extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
void UnloadBlueCoinModel(void *);
extern int data_ov096_02137b20[];
extern int data_ov096_02137b28[];
int _ZN5Pokey16CleanupResourcesEv(char *c){
  int id = *(unsigned short*)(c+0xc);
  int a = (id == 0xf0);
  if (a) {
    UnloadBlueCoinModel(c);
    _ZN13SharedFilePtr7ReleaseEv(data_ov096_02137b20);
    _ZN13SharedFilePtr7ReleaseEv(data_ov096_02137b28);
  } else {
    a = (id == 0xf1);
    if (a) {
      _ZN13SharedFilePtr7ReleaseEv(data_ov096_02137b28);
    }
  }
  return 1;
}
}
