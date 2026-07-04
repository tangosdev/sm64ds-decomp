extern int _ZN5Actor13DistToCPlayerEv(void*);
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned,unsigned,unsigned,int,int);
extern int DecIfAbove0_Short(void*);
extern int _ZN5Actor24KillAndTrackInDeathTableEv(void*);
int func_ov019_02112678(char* c){
  switch (*(unsigned char*)(c+0xd6)) {
  case 0:
    if (_ZN5Actor13DistToCPlayerEv(c) < 0x180000) {
      unsigned char* p = (unsigned char*)(((int)c + 0xd6) & 0xFFFFFFFFFFFFFFFF);
      _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x14, 0x7f, 0x15666, 0);
      *p += 1;
    }
    break;
  case 1:
    if (DecIfAbove0_Short((char*)c+0xd4) == 0) {
      _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x7f, 0, 0x15666, 0);
      _ZN5Actor24KillAndTrackInDeathTableEv(c);
    }
    break;
  }
  return 1;
}
