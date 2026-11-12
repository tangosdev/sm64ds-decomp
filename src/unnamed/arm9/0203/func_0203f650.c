#include "private/arm9_a0f90_channel_measure.h"

/* Report the channel measurement's outcome to the caller's callback
   (data_020a0f90, set by func_0203f9d4): a positive x is the chosen channel,
   sent with result 0; zero or a negative error code is sent as the result
   with channel 0. */
void func_0203f650(int x) {
    ChannelMeasureReply reply;
    ChannelMeasureCallback fn = data_020a0f90;
    if (fn == 0)
        return;
    if (x > 0) {
        reply.result = 0;
        reply.channel = (unsigned short)x;
    } else {
        reply.result = (short)x;
        reply.channel = 0;
    }
    fn(&reply);
}
