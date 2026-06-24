//cpp
extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void *);
void UnloadBlueCoinModel(void);
extern int data_ov002_0210da38;
extern int data_ov071_02123050;
extern int* data_ov071_021226a4;
extern void* data_ov071_021226a0;
int func_ov071_02121650(void){
  int i;
  UnloadBlueCoinModel();
  _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210da38);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov071_02123050);
  for(i=0;i<2;i++){
    _ZN13SharedFilePtr7ReleaseEv((&data_ov071_021226a4)[i]);
  }
  _ZN13SharedFilePtr7ReleaseEv(data_ov071_021226a0);
  return 1;
}
}
