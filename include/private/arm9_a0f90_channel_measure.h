#ifndef PRIVATE_ARM9_A0F90_CHANNEL_MEASURE_H
#define PRIVATE_ARM9_A0F90_CHANNEL_MEASURE_H

/*
 * The wireless channel measurement func_0203e20c starts with func_0203f9d4
 * (the shape of the NitroSDK wireless sample library's): the reply it ends
 * with, and the caller's callback that receives it, which data_020a0f90
 * keeps until WM_End. Coined names; plain C types.
 */

/* result is 0 with the chosen channel, or a negative error with channel 0. */
typedef struct ChannelMeasureReply {
    short result;           /* 0x0 */
    unsigned short channel; /* 0x2 */
} ChannelMeasureReply;

typedef void (*ChannelMeasureCallback)(ChannelMeasureReply *);

#ifdef __cplusplus
extern "C" {
#endif

extern ChannelMeasureCallback data_020a0f90;

void func_0203f9d4(void *, ChannelMeasureCallback);

#ifdef __cplusplus
}
#endif

#endif
