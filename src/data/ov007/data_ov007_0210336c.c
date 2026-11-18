/* 0x0210336c: the overlay's file-release hook, bound by func_ov007_020b3edc
 * and called per released file. */
#include "types.h"

void (*data_ov007_0210336c)(void *file);
