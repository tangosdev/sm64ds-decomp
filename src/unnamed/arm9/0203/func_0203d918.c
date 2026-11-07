#include "private/player_input_record.h"

/* Raise flag 0x4000 of this console's input record, the load request:
   once every present child has it, player 0 sets 0x2000 in its own record
   (func_0203ea5c), which func_0203d8fc reads. dScEntry_c::EnterMatchLoading
   and func_0203db64 raise it; func_0203d930 drops it. */
void func_0203d918(void) {
    data_020a1040.flags |= 0x4000;
}
