//cpp
extern "C" {
void _ZN13SharedFilePtr7ReleaseEv(void* fp);
}
extern void* data_ov002_020ff0ac[];
extern void* data_ov002_0210de50;
extern void* data_ov002_0210de60;
extern void* data_ov002_0210de48;
extern void* data_ov002_0210de28;
extern void* data_ov002_0210de08;
extern void* data_ov002_0210de20;
extern void* data_ov002_0210de40;
extern void* data_ov002_0210de10;
extern void* data_ov002_0210de00;
extern void* data_ov002_0210de58;
extern void* data_ov002_0210de18;
extern void* data_ov002_0210de30;
extern void* data_ov002_0210de38;
extern "C" int _ZN13WaterfallMist16CleanupResourcesEv(char* c){
  int i = *(int*)(c + 0x3f4);
  if (i >= 3) return 1;
  _ZN13SharedFilePtr7ReleaseEv(data_ov002_020ff0ac[i]);
  switch (*(int*)(c + 0x3f0)) {
  case 0xf:
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210de50);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210de60);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210de48);
    break;
  case 0x14:
  case 0x15:
  case 0x16:
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210de28);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210de08);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210de20);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210de40);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210de10);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210de00);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210de58);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210de18);
    break;
  default:
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210de30);
    _ZN13SharedFilePtr7ReleaseEv(&data_ov002_0210de38);
    break;
  }
  return 1;
}
