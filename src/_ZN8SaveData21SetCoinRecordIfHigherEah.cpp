//cpp
// @symbol _ZN8SaveData21SetCoinRecordIfHigherEah
#include "SaveData.h"

/* SaveData::SetCoinRecordIfHigher(s8 courseID, u8 coins) at 0x02013650 -- static.
 *
 * The write side of GetCoinRecord: keeps the best coin count per main level.
 *
 * The parameter types are read off the mangled name, not chosen -- `Eah` is
 * (signed char, unsigned char). The earlier hand-spelled version declared the
 * first parameter `int`, which was harmless only because nothing derived the
 * symbol from it; as a real method that spelling would mangle to `Ei` and name a
 * different function.
 */
extern "C" u8 data_0209cad2[]; /* &SAVE_DATA.coinRecords[0]; no friendly symbol yet */

void SaveData::SetCoinRecordIfHigher(s8 courseID, u8 coins)
{
    if (coins > data_0209cad2[courseID])
        data_0209cad2[courseID] = coins;
}
