// @symbol _ZN8SaveData21SetCoinRecordIfHigherEah
/* recovered: named members + shared header, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header */
#include "SaveData.h"
void _ZN8SaveData21SetCoinRecordIfHigherEah(int a, unsigned char b){
  if (b > data_0209cad2[a])
    data_0209cad2[a] = b;
}
