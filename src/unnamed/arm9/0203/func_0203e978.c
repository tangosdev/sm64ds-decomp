#include "private/arm9_a0f90_channel_measure.h"

extern unsigned short data_020a0f28;
extern unsigned char data_020a0f0c;

/* The callback func_0203e20c hands func_0203f9d4: on success, keep the
   channel in data_020a0f28 (as func_020408b0 sets it from the parent's
   WMBssDesc) and raise data_020a0f0c. */
void func_0203e978(ChannelMeasureReply *reply) {
    if (reply->result != 0)
        return;
    data_020a0f28 = reply->channel;
    data_020a0f0c = 1;
}
