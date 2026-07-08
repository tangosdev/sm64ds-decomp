//cpp
extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void*);
void UnloadBlueCoinModel(void*);
extern int data_ov084_02130dfc;
extern void *data_ov085_021302f4[];
extern int BUBBLE_MODEL_PTR;
int func_ov084_0212e554(void *c) {
  _ZN13SharedFilePtr7ReleaseEv(&data_ov084_02130dfc);
  for (int i = 0; i < 6; i++) {
    _ZN13SharedFilePtr7ReleaseEv(data_ov085_021302f4[i]);
  }
  _ZN13SharedFilePtr7ReleaseEv(&BUBBLE_MODEL_PTR);
  UnloadBlueCoinModel(c);
  return 1;
}
}
