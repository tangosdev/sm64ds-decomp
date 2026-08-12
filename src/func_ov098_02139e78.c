// @symbol func_ov098_02139e78
// recovered name: Crate_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* daObjBlockS_c::OnTurnIntoEgg
 * func_ov098_02139e78 at 0x02139e78
 * Byte-matched with mwccarm 2004/b56 (ov098), strict-reloc verified.
 */
extern int _ZN6Player15IsCollectingCapEv(char* player);
extern void _ZN5Actor15GivePlayerCoinsER6Playerhj(char* self, char* player, unsigned char n, unsigned int j);
extern void _ZN6Player20RegisterEggCoinCountEjbb(char* player, unsigned int n, char b1, char b2);
extern void Crate_SetState(char* c, int i);

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
  Crate_SetState(r5, 6);
}
