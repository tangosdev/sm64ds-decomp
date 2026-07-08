//cpp
extern "C" {
int _ZN6Player7IsStateERNS_5StateE(void* p, void* st);
int func_ov002_020dacb4(void* p, void* actor);
}
extern char _ZN6Player13ST_PUNCH_KICKE;
extern char _ZN6Player7ST_SWIME;
extern char _ZN6Player7ST_DIVEE;
extern "C" int _ZN6Player7TryGrabER5Actor(char* p, char* actor){
  int b = (*(int*)(p + 0x358) != 0);
  if (b) return 0;
  if ((*(int*)(actor + 0xb0) & 0x80) == 0) return 0;
  if (!(_ZN6Player7IsStateERNS_5StateE(p, &_ZN6Player13ST_PUNCH_KICKE) && *(unsigned char*)(p + 0x6e2) != 2)){
    if (!_ZN6Player7IsStateERNS_5StateE(p, &_ZN6Player7ST_SWIME)) goto bd;
  }
  func_ov002_020dacb4(p, actor);
  return 1;
bd:
  {
    unsigned short t = *(unsigned short*)(actor + 0xc);
    int b0 = (t == 0xbd);
    if (!b0){
      int b1 = (t == 0xbe);
      if (!b1){
        if (_ZN6Player7IsStateERNS_5StateE(p, &_ZN6Player7ST_DIVEE)){
          func_ov002_020dacb4(p, actor);
          return 1;
        }
      }
    }
  }
  return 0;
}
