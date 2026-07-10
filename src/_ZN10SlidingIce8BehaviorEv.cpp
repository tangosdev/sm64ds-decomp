//cpp
struct V3 { int x,y,z; };
struct V316 { short x,y,z; };
extern "C" {
extern int DecIfAbove0_Short(void*);
extern int DecIfAbove0_Byte(void*);
extern int _Z14ApproachLinearRiii(int*, int, int);
extern void _ZN9ActorBase18MarkForDestructionEv(void*);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void*, void*);
extern int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int, unsigned int, unsigned int, void*, unsigned int);
extern int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int, unsigned int, void*, void*, int, int);
int _ZN10SlidingIce8BehaviorEv(char *c){
  int isType = (*(unsigned short*)(c+0xc) == 0x5d);
  if(isType){
    if(DecIfAbove0_Short(c+0x31e) == 0){
      _Z14ApproachLinearRiii((int*)(c+0x98), 0, 0x3000);
      if(_Z14ApproachLinearRiii((int*)(c+0x60), *(int*)(c+0x324), 0xa000) != 0){
        _ZN9ActorBase18MarkForDestructionEv(c);
      }
    }
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    *(int*)(c+0x328) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int*)(c+0x328), 3, 0x98, c+0x74, 0);
  } else {
    if(DecIfAbove0_Short(c+0x31e) == 0){
      V3 pos;
      pos.x = *(int*)(c+0x5c);
      pos.y = *(int*)(c+0x60);
      pos.z = *(int*)(c+0x64);
      int spawnType = 1;
      if(DecIfAbove0_Byte(c+0x320) == 0){
        *(char*)(c+0x320) = 5;
        spawnType = 2;
      } else {
        pos.y -= 0x50000;
      }
      unsigned char cnt = *(unsigned char*)(c+0x320);
      *(short*)(c+0x31e) = (cnt + 1) * 0x14;
      _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0x5d, spawnType, &pos, c+0x8c, *(signed char*)(c+0xcc), -1);
    }
  }
  return 1;
}
}
