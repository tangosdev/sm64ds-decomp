//cpp
// @symbol _ZN3HUD14RenderRedCoinsEv
/* recovered: shared header, real C++ method
 *
 * Draws one sprite per red coin still to collect, stepping 0xb across from
 * x=0x10. Both the count (data_0209f30c) and the row selector (data_0209f310)
 * are indexed by the same data_0209f250[0].
 *
 * That row selector is the interesting bit: it is the SAME byte
 * HUD::RenderSilverStars reads as its star COUNT. Non-zero there means silver
 * stars are being drawn at y=0xa, so the red coins drop to y=0x1c to make room.
 * One byte serving as both a count and an occupancy flag for a shared row.
 *
 * The count is read through an explicit unsigned char cast off a signed char
 * array -- kept as the ROM spells it, since the same array is read signed by
 * RenderSilverStars.
 *
 * TOUCHES NO FIELD; the ROM body takes no `this` and now ignores the one it is
 * passed. Byte-free.
 */
#include "HUD.h"

struct OamAttr; struct Matrix2x2;
extern "C" {
extern unsigned char data_0209f250[];
extern signed char data_0209f310[];
extern signed char data_0209f30c[];
extern struct OamAttr data_ov002_0210c6b8;
void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(bool, struct OamAttr*, int, int, int, int, struct Matrix2x2*);
}

void HUD::RenderRedCoins()
{
  int idx = data_0209f250[0];
  int flag = data_0209f310[idx];
  int count = (unsigned char)data_0209f30c[idx];
  int sl = flag ? 0x1c : 0xa;
  for(int i=0;i<count;i++){
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(false, &data_ov002_0210c6b8, 0x10 + i*0xb, sl, -1, 1, (struct Matrix2x2*)0);
  }
}
