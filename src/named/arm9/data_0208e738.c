#include "types.h"

/* AddToCommonModelDataArr reads this flag. The function now lives in
 * Model.cpp, and this word is the only data that file does not own. */
#pragma define_section forcedata ".data" ".data" ".data" data_mode=far_abs
#pragma section forcedata begin
int data_0208e738[1] = { 0x00000001 };
#pragma section forcedata end
