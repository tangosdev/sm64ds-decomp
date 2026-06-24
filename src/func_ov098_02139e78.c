extern int _ZN6Player15IsCollectingCapEv(char* player);
extern void _ZN5Actor15GivePlayerCoinsER6Playerhj(char* self, char* player, unsigned char n, unsigned int j);
extern void _ZN6Player20RegisterEggCoinCountEjbb(char* player, unsigned int n, char b1, char b2);
extern void func_ov098_02138b28(char* c, int i);

void func_ov098_02139e78(char* r5, char* r4){
  if (_ZN6Player15IsCollectingCapEv(r4)) {
    if (*(unsigned char*)(r5 + 0x607) != 1) {
      _ZN5Actor15GivePlayerCoinsER6Playerhj(r5, r4, 3, 0);
      *(unsigned char*)(r5 + 0x607) = 1;
    }
  } else {
    unsigned int count = 0;
    if (*(unsigned char*)(r5 + 0x607) != 1) {
      *(unsigned char*)(r5 + 0x607) = 1;
      count = 3;
    }
    _ZN6Player20RegisterEggCoinCountEjbb(r4, count, 0, 0);
  }
  func_ov098_02138b28(r5, 6);
}
