#include "private/player_input_record.h"

/* daPly_c idx's touch sample (x, y, touch, validity at +0x4); func_0203bb60
   and the DSMT scene read it. */
TPData *func_0203dabc(int idx) {
    return &data_020a1154[idx].touch;
}
