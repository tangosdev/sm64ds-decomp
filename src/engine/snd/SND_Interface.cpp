//cpp
/* The ARM9 side of the sound driver's command interface (the Nitro SDK's
 * SND_interface.c), arm9 .text 0x0205a86c..0x0205adf8.
 *
 * Each call packs its arguments into one SNDCommand from SND_command.c's
 * pool and queues it for the ARM7 driver. SNDi_SetPlayerParam and
 * SNDi_SetTrackParam write one field of the driver's SNDPlayer or SNDTrack;
 * the player and track setters are those two with the field's offset and
 * size.
 *
 * pret's pokediamond SND_interface.c defines these in this order, with
 * PushCommand_impl last. mwccarm emits the bodies in reverse, so the ROM
 * runs from PushCommand_impl at 0x0205a86c up to SND_StartSeq at 0x0205adc4.
 * The linker dropped what the game never calls. This SDK is older than
 * pret's, and its command numbers are lower (see SND_CMD_* below).
 * SND_SetTrackModDepth and SND_SetTrackModSpeed, which pret's file leaves
 * out, sit after SND_SetTrackPan in the order of pret's SND_interface.h.
 */
#include "types.h"
#include "nitro/snd/alarm.h"
#include "nitro/snd/command.h"
#include "nitro/snd/interface.h"

/* This SDK's command numbers. There is no SND_CMD_PREPARE_SEQ,
 * SND_CMD_START_PREPARED_SEQ or SND_CMD_SKIP_SEQ, so pause is 2 and
 * pret's 6..0x17 are 3 lower here. There is no SND_CMD_MASTER_PAN, so
 * pret's 0x19..0x1d are 4 lower. The invalidate commands are 3 lower
 * again. */
#define SND_CMD_START_SEQ 0x00
#define SND_CMD_STOP_SEQ 0x01
#define SND_CMD_PAUSE_SEQ 0x02
#define SND_CMD_PLAYER_PARAM 0x03
#define SND_CMD_TRACK_PARAM 0x04
#define SND_CMD_MUTE_TRACK 0x05
#define SND_CMD_START_TIMER 0x09
#define SND_CMD_STOP_TIMER 0x0a
#define SND_CMD_SETUP_CHANNEL_PCM 0x0b
#define SND_CMD_SETUP_CAPTURE 0x0e
#define SND_CMD_SETUP_ALARM 0x0f
#define SND_CMD_CHANNEL_VOLUME 0x11
#define SND_CMD_SURROUND_DECAY 0x13
#define SND_CMD_OUTPUT_SELECTOR 0x15
#define SND_CMD_LOCK_CHANNEL 0x16
#define SND_CMD_UNLOCK_CHANNEL 0x17
#define SND_CMD_INVALIDATE_SEQ 0x1b
#define SND_CMD_INVALIDATE_BANK 0x1c
#define SND_CMD_INVALIDATE_WAVE 0x1d

#define SND_CMD_FLAG_BLOCK 0x1
#define SND_ALARM_COUNT 8

/* SNDPlayer and SNDTrack field offsets, from pret's SND_seq_shared.h. */
#define SND_PLAYER_PRIO 0x04
#define SND_PLAYER_EXT_FADER 0x06
#define SND_PLAYER_TEMPO_RATIO 0x1a
#define SND_TRACK_EXT_PAN 0x09
#define SND_TRACK_EXT_FADER 0x0a
#define SND_TRACK_EXT_PITCH 0x0c
#define SND_TRACK_MOD_SPEED 0x19
#define SND_TRACK_MOD_DEPTH 0x1a

struct SNDBankData;

SNDCommand *SND_AllocCommand(u32 flags);
void SND_PushCommand(SNDCommand *cmd);
void SNDi_IncAlarmId(u32 idx);
u8 SNDi_SetAlarmHandler(u32 idx, SNDAlarmCallback cb, void *data);

void PushCommand_impl(s32 id, u32 par1, u32 par2, u32 par3, u32 par4);
void SNDi_SetPlayerParam(s32 player, u32 offset, u32 data, s32 size);
void SNDi_SetTrackParam(s32 player, u32 trackBitMask, u32 offset, u32 data, s32 size);

void SND_StartSeq(s32 player, const void *seqBasePtr, u32 seqOffset, SNDBankData *bankData) {
    PushCommand_impl(SND_CMD_START_SEQ, (u32)player, (u32)seqBasePtr, seqOffset, (u32)bankData);
}

void SND_StopSeq(s32 player) {
    PushCommand_impl(SND_CMD_STOP_SEQ, (u32)player, 0, 0, 0);
}

void SND_PauseSeq(s32 player, int flag) {
    PushCommand_impl(SND_CMD_PAUSE_SEQ, (u32)player, (u32)flag, 0, 0);
}

void SND_SetPlayerTempoRatio(s32 player, s32 ratio) {
    SNDi_SetPlayerParam(player, SND_PLAYER_TEMPO_RATIO, (u32)ratio, 2);
}

void SND_SetPlayerVolume(s32 player, s32 volume) {
    SNDi_SetPlayerParam(player, SND_PLAYER_EXT_FADER, (u32)volume, 2);
}

void SND_SetPlayerChannelPriority(s32 player, s32 prio) {
    SNDi_SetPlayerParam(player, SND_PLAYER_PRIO, (u32)prio, 1);
}

void SND_SetTrackVolume(s32 player, u32 trackBitMask, s32 volume) {
    SNDi_SetTrackParam(player, trackBitMask, SND_TRACK_EXT_FADER, (u32)volume, 2);
}

void SND_SetTrackPitch(s32 player, u32 trackBitMask, s32 pitch) {
    SNDi_SetTrackParam(player, trackBitMask, SND_TRACK_EXT_PITCH, (u32)pitch, 2);
}

void SND_SetTrackPan(s32 player, u32 trackBitMask, s32 pan) {
    SNDi_SetTrackParam(player, trackBitMask, SND_TRACK_EXT_PAN, (u32)pan, 1);
}

void SND_SetTrackModDepth(s32 player, u32 trackBitMask, s32 depth) {
    SNDi_SetTrackParam(player, trackBitMask, SND_TRACK_MOD_DEPTH, (u32)depth, 1);
}

void SND_SetTrackModSpeed(s32 player, u32 trackBitMask, s32 speed) {
    SNDi_SetTrackParam(player, trackBitMask, SND_TRACK_MOD_SPEED, (u32)speed, 1);
}

void SND_StartTimer(u32 chnBitMask, u32 capBitMask, u32 alarmBitMask, u32 flags) {
    PushCommand_impl(SND_CMD_START_TIMER, chnBitMask, capBitMask, alarmBitMask, flags);
}

void SND_StopTimer(u32 chnBitMask, u32 capBitMask, u32 alarmBitMask, u32 flags) {
    s32 i = 0;
    u32 tmpMask = alarmBitMask;

    while (i < SND_ALARM_COUNT && tmpMask != 0) {
        if (tmpMask & 1)
            SNDi_IncAlarmId((u32)i);
        i++;
        tmpMask >>= 1;
    }

    PushCommand_impl(SND_CMD_STOP_TIMER, chnBitMask, capBitMask, alarmBitMask, flags);
}

void SND_SetupCapture(s32 capture, s32 format, void *bufferPtr, u32 length, int loopFlag, s32 in,
                      s32 out) {
    PushCommand_impl(
        SND_CMD_SETUP_CAPTURE, (u32)bufferPtr, length,
        (u32)((capture << 31) | (format << 30) | (loopFlag << 29) | (in << 28) | (out << 27)), 0);
}

void SND_SetupAlarm(s32 alarm, u32 tick, u32 period, SNDAlarmCallback cb, void *userData) {
    PushCommand_impl(SND_CMD_SETUP_ALARM, (u32)alarm, tick, period,
                     SNDi_SetAlarmHandler((u32)alarm, cb, userData));
}

void SND_SetTrackMute(s32 player, u32 trackBitMask, int flag) {
    PushCommand_impl(SND_CMD_MUTE_TRACK, (u32)player, trackBitMask, (u32)flag, 0);
}

void SND_LockChannel(u32 chnBitMask, u32 flags) {
    PushCommand_impl(SND_CMD_LOCK_CHANNEL, chnBitMask, flags, 0, 0);
}

void SND_UnlockChannel(u32 chnBitMask, u32 flags) {
    PushCommand_impl(SND_CMD_UNLOCK_CHANNEL, chnBitMask, flags, 0, 0);
}

void SND_SetChannelVolume(u32 chnBitMask, s32 volume, s32 chnDataShift) {
    PushCommand_impl(SND_CMD_CHANNEL_VOLUME, chnBitMask, (u32)volume, (u32)chnDataShift, 0);
}

void SND_SetupChannelPcm(s32 chn, s32 waveFormat, const void *dataAddr, s32 loopMode, s32 loopStart,
                         s32 dataLen, s32 volume, s32 chnDataShift, s32 timer, s32 pan) {
    PushCommand_impl(SND_CMD_SETUP_CHANNEL_PCM, (u32)(chn | (timer << 16)), (u32)dataAddr,
                     (u32)((volume << 24) | (chnDataShift << 22) | dataLen),
                     (u32)((loopMode << 26) | (waveFormat << 24) | (pan << 16) | loopStart));
}

void SND_InvalidateSeqData(const void *start, const void *end) {
    PushCommand_impl(SND_CMD_INVALIDATE_SEQ, (u32)start, (u32)end, 0, 0);
}

void SND_InvalidateBankData(const void *start, const void *end) {
    PushCommand_impl(SND_CMD_INVALIDATE_BANK, (u32)start, (u32)end, 0, 0);
}

void SND_InvalidateWaveData(const void *start, const void *end) {
    PushCommand_impl(SND_CMD_INVALIDATE_WAVE, (u32)start, (u32)end, 0, 0);
}

void SND_SetOutputSelector(s32 left, s32 right, s32 channel1, s32 channel3) {
    PushCommand_impl(SND_CMD_OUTPUT_SELECTOR, (u32)left, (u32)right, (u32)channel1, (u32)channel3);
}

void SNDi_SetPlayerParam(s32 player, u32 offset, u32 data, s32 size) {
    PushCommand_impl(SND_CMD_PLAYER_PARAM, (u32)player, offset, data, (u32)size);
}

void SNDi_SetTrackParam(s32 player, u32 trackBitMask, u32 offset, u32 data, s32 size) {
    PushCommand_impl(SND_CMD_TRACK_PARAM, (u32)(player | (size << 24)), trackBitMask, offset, data);
}

void SNDi_SetSurroundDecay(s32 decay) {
    PushCommand_impl(SND_CMD_SURROUND_DECAY, (u32)decay, 0, 0, 0);
}

void PushCommand_impl(s32 id, u32 par1, u32 par2, u32 par3, u32 par4) {
    SNDCommand *cmd = SND_AllocCommand(SND_CMD_FLAG_BLOCK);
    if (cmd == 0)
        return;

    cmd->id = id;
    cmd->arg[0] = par1;
    cmd->arg[1] = par2;
    cmd->arg[2] = par3;
    cmd->arg[3] = par4;
    SND_PushCommand(cmd);
}
