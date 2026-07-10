extern int _ZN13SharedFilePtr7ReleaseEv(void*);
extern void UnloadSilverStarAndNumber(void);
extern char data_ov002_0210d9d8[];
extern char data_ov002_0210da30[];
extern char data_ov002_0210da18[];
int _ZN10BrickBlock16CleanupResourcesEv(char* c){
  int v = *(unsigned short*)(c+0xc);
  switch(v){
  case 0x141: _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210d9d8); break;
  case 0x142: _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da30); break;
  case 0x143: _ZN13SharedFilePtr7ReleaseEv(data_ov002_0210da18); break;
  case 0x144: UnloadSilverStarAndNumber(); break;
  }
  return 1;
}
