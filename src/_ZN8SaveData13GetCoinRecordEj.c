//cpp
// @symbol _ZN8SaveData13GetCoinRecordEj
#include "SaveData.h"

/* SaveData::GetCoinRecord(u32 courseID) at 0x0201366c -- static, no `this`.
 *
 * Returns the saved coin count for a main level. The records are a u8 array
 * inside FileSaveData; the reloc resolves to 0x0209cad2, which is
 * &SAVE_DATA.coinRecords[0]. That address has no friendly symbol in
 * symbols.txt yet and the reloc is a wildcard pooled global, so this extern
 * name is not itself byte-verified -- only the indexing is.
 */
extern "C" u8 data_0209cad2[]; /* &SAVE_DATA.coinRecords[0] */

u8 SaveData::GetCoinRecord(u32 courseID)
{
    return data_0209cad2[courseID];
}
