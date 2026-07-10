//cpp
extern "C" {
extern void UnloadKeyModels(int i);
extern void _ZN13SharedFilePtr7ReleaseEv(void*);
extern void* data_ov073_02123280;
extern void* data_ov073_021232a0;
extern void* data_ov073_02123288;
extern void* data_ov073_021232a8;
extern void* data_ov073_02123290;
extern void* data_ov073_021232b0;
extern void* data_ov073_021232b8;
extern void* data_ov002_0210da30;
extern void* data_ov073_02123298;

int _ZN11ChiefChilly16CleanupResourcesEv(void){
  UnloadKeyModels(4);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov073_02123280);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov073_021232a0);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov073_02123288);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov073_021232a8);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov073_02123290);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov073_021232b0);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov073_021232b8);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210da30);
  _ZN13SharedFilePtr7ReleaseEv(&data_ov073_02123298);
  return 1;
}
}
