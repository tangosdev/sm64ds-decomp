#include "private/player_input_record.h"

/* The input record's frame counter (data_020a1040.frame, counted up by
   func_0203df40). The VS stage seeds from it. */
int func_0203dad4(void) {
    return data_020a1040.frame;
}
