//cpp
struct C;
typedef void (C::*PMF)();
extern "C" PMF data_ov002_0210dd30[];
extern "C" {
void* _ZN5Actor4NextEPKS_(const void* prev);
int Vec3_Dist(const void* a, const void* b);
void _ZN9ActorBase18MarkForDestructionEv(void* c);
}
struct C { char pad[0x1000]; };
extern "C" int _ZN10BrickBlock8BehaviorEv(char* c){
  char* o = 0;
  if (*(unsigned char*)(c + 0xd8) != 0) goto d6;
  o = (char*)_ZN5Actor4NextEPKS_(0);
  while (o){
    unsigned short t = *(unsigned short*)(o + 0xc);
    int b;
    b = (t == 0x10);
    if (!b){
      b = (t == 0xf);
      if (!b){
        b = (t == 0x11);
        if (!b) goto next;
      }
    }
    if (Vec3_Dist(c + 0x5c, o + 0x5c) < 0x32000){
      *(char**)(o + 0x328) = c;
      *(unsigned char*)(c + 0xd8) = 1;
      return 1;
    }
  next:
    o = (char*)_ZN5Actor4NextEPKS_(o);
  }
  if (o) goto d6;
  _ZN9ActorBase18MarkForDestructionEv(c);
  return 1;
d6:
  if (*(unsigned char*)(c + 0xd6) != 0){
    int idx = *(unsigned char*)(c + 0xd7);
    (((C*)c)->*data_ov002_0210dd30[idx])();
    _ZN9ActorBase18MarkForDestructionEv(c);
  }
  return 1;
}
