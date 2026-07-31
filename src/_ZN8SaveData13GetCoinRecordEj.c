#include "types.h"
// @symbol _ZN8SaveData13GetCoinRecordEj
/* recovered: named members + shared header */
#include "SaveData.h"
/* SaveData::GetCoinRecord(u32 courseID) at 0x0201366c
 * Static method (no `this`); returns the saved coin count for a main level.
 * Reads SAVE_DATA.coinRecords[courseID] -- a u8 array. See Save.h:
 *   u8 coinRecords[NUM_MAIN_LEVELS];  // FileSaveData
 *
 * The reloc resolves to 0x0209cad2 (= &SAVE_DATA.coinRecords[0]); the friendly
 * symbol is not yet in symbols.txt (wildcard pooled-global reloc, so this
 * extern name is not byte-verified). Kept as a named u8[] over data_0209cad2.
 */
extern u8 SAVE_DATA_COIN_RECORDS[]; /* &SAVE_DATA.coinRecords[0], 0x0209cad2 */

u8 _ZN8SaveData13GetCoinRecordEj(u32 courseID)
{
    return SAVE_DATA_COIN_RECORDS[courseID];
}
