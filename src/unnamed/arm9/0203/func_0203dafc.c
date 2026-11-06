#include "private/player_input_record.h"

/* Store the camera's input yaw in the input record (data_020a1040.inputYaw);
   dCamera_c::Behavior passes mAngleY + mInputYawLag. */
void func_0203dafc(int v) {
    data_020a1040.inputYaw = v;
}
