// SSEQ bytecode sequencer.
//
// 16 players (the DS has 16 too), 16 tracks each, driven at 192 Hz by
// sd_mix_render. Tempo is BPM against 48 ticks per quarter note: the tick
// counter takes +tempo per 192 Hz frame and fires a tick at 240, which is
// 192*tempo/240 = tempo*48/60 ticks per second. That is the DS's timing.
//
// SM64DS's sound effects are sequences too -- SDAT has no "raw sample"
// concept -- so this file is on the critical path for SFX, not just music.
//
// Unimplemented opcodes print once by id and end the track that hit one.
// Ending is deliberate: an unknown opcode has an unknown argument length, so
// skipping it would desync the stream and play garbage. Silence that says
// why beats noise that lies.
#include "sdat.h"

#include <math.h>
#include <chrono>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace {

enum { SD_PLAYERS = 16, SD_TRACKS = 16, SD_STACK = 4, SD_VARS = 32 };

/* ---- the pitch dump ------------------------------------------------------
 *
 * SM64DS_PITCH_DUMP=<path> (or =1 for stderr). One line per voice start and
 * one per pitch change, carrying every input the DS's own pitch computation
 * takes, in the DS's own 1/64-semitone units. It exists because an off-key
 * report cannot be answered by listening: the answer is a per-voice diff
 * against the driver the ROM's ARM7 actually runs, and this is the side of
 * that diff the port can supply.
 *
 * Latched once, off by default, every call site behind SD_PD, so with the
 * variable unset it costs one predictable branch.
 */
int g_pdump;                    // 0 = off, 1 = armed
FILE *g_pdumpFile;
int g_frame;                    // 192 Hz sequencer frames since sd_seq_reset

void sd_pdump(const char *fmt, ...)
{
    if (!g_pdumpFile) return;
    va_list ap;
    va_start(ap, fmt);
    fputs("[pd] ", g_pdumpFile);
    vfprintf(g_pdumpFile, fmt, ap);
    va_end(ap);
}

#define SD_PD(...) do { if (g_pdump) sd_pdump(__VA_ARGS__); } while (0)

void pdump_open(void)
{
    static int latched;
    if (latched) return;
    latched = 1;
    const char *p = getenv("SM64DS_PITCH_DUMP");
    if (!p || !*p) return;
    if (!strcmp(p, "1") || !strcmp(p, "-")) {
        g_pdumpFile = stderr;
    } else {
        g_pdumpFile = fopen(p, "w");
        if (!g_pdumpFile) {
            fprintf(stderr, "[pd] cannot open %s -- pitch dump off\n", p);
            return;
        }
    }
    g_pdump = 1;
    fprintf(g_pdumpFile,
            "[pd] units are 1/64 of a semitone, 768 to the octave. "
            "pu = (bend * (range << 6)) >> 7 + ext, the ROM's "
            "TrackUpdateChannel. rate is sample frames per output frame at "
            "%d Hz.\n", sd_mix_rate());
}

struct Track {
    int active;
    sd_u32 pc;
    sd_u32 stack[SD_STACK];
    int sp;
    sd_u32 loopPc[SD_STACK];
    int loopCount[SD_STACK];
    int loopSp;

    int wait;               // ticks still to burn before the next command
    int prog;               // program (instrument) number
    int volume, expression, pan, panSet;
    // bend and bendRange are the two halves of ONE per-frame quantity, not a
    // per-note one. See track_pitch_units below: the ROM recomputes them into
    // the sounding channels every frame, so a PITCHBEND written while a note
    // is still ringing bends THAT note. Reading them only at the note-on left
    // the slide theme's lead a full semitone flat for 17.8% of its sounding
    // time; the measurement is on track_pitch_units.
    int transpose, bend, bendRange;
    int priority;
    // TRACK_PARAM 0x0a and 0x0c: the two halves of the ARM7's per-track
    // trim, and the per-track twins of the two player fields above them.
    // Both arrive WHILE the sound plays and both reach notes already
    // sounding, for sd_seq_set_volume_db10's reason.
    //
    //   volDb10  a SIGNED attenuation in tenths of a decibel, out of the
    //            ROM's own 128-entry table data_02086384 (-723..0). Same
    //            units, same table and same clamp as PLAYER_PARAM 6:
    //            func_0204f82c indexes data_02086384 to build it and
    //            func_0204fafc indexes the same table to build the player's.
    //   pitch    a SIGNED offset in 1/64 of a semitone, the DS pitch domain
    //            this file already works in (768 units to the octave; see
    //            the bend arithmetic in start_note). func_ov007_020bdbcc
    //            sweeps it over -0x300..0, exactly one octave down.
    int volDb10;
    int pitch;
    /* PORTAMENTO AND SWEEP, the track half (SM64DS ARM7 track +0x14 portaKey,
     * +0x15 portaTime, +0x16 sweepPitch, flags bit 5 portamento; TrackInit at
     * 0x037FDA74 sets the key to 60 and the rest to 0). A note-on hands the
     * channel a sweep made of them and the channel plays it out on its own;
     * see note_sweep. */
    int portaKey, portaOn, portaTime, sweepPitch;
    /* MODULATION, the track's LFO parameters (track +0x18 target, +0x19
     * speed, +0x1a depth, +0x1b range, +0x1c delay; TrackInit's 0x037FBFBC:
     * target 0, speed 16, depth 0, range 1, delay 0). Written by 0xCA-0xCD /
     * 0xE0 and by TRACK_PARAM 0x19 / 0x1a from the game, copied onto the
     * track's sounding channels every frame; the channel runs the LFO. */
    int modTarget, modSpeed, modDepth, modRange, modDelay;
    /* THE TRACK'S ENVELOPE OVERRIDE (track +0x0e attack, +0x0f decay, +0x10
     * sustain, +0x11 release; 0xFF = use the instrument's, TrackInit's
     * 0x037FDB0C). 0xD0..0xD3 set them and every note-on after that hands
     * each one that is not 0xFF to its channel (0x037FD568..0x037FD5B4),
     * over whatever the instrument's own ADSR said. */
    int envAttack, envDecay, envSustain, envRelease;
    int noteWait;           // C7: notes block the track for their duration
    // A note played with duration 0 under noteWait blocks the track until the
    // CHANNELS it owns have ended, rather than for a tick count. That is what
    // keeps the SEQARC one-shots ("program change, note, end of track")
    // sounding: the track does not reach its 0xff until the sample is done.
    // SND_seq.c calls it noteFinishWait; it is set at the note-on (line 939)
    // and cleared at the top of the track's main loop (lines 867 to 871).
    int noteFinishWait;
    int tie;
    int cmpFlag;
    int mono;
    /* Allocated by the sequence's leading 0xFE mask at START and not opened
       yet. The ARM7 builds those tracks (TrackInit) inside the START command
       (0x037FE394), BEFORE the batch's TRACK_PARAM / MUTE commands run, and
       the 0x93 open-track that comes later (0x037FCF44..0x037FCF6C) only
       points the already-existing track at its code: no TrackInit, so a
       volume or mute the game sent in the meantime survives. */
    int allocd;
};

struct Player {
    int active;
    const sd_u8 *seq;       // bytecode base (SSEQ base + *(u32*)(base+0x18))
    const sd_u8 *sbnk;
    Track tr[SD_TRACKS];
    int tempo, tempoCount;
    int volume, pan;
    int volDb10;            // PLAYER_PARAM 6: distance/fade attenuation
    // The sequence's base CHANNEL PRIORITY, out of the SDAT's cpr byte. Every
    // track of this player starts here (sd_seq_start and 0x93 open-track); the
    // 0xC6 opcode overrides it per track. See sdat_seq_priority: defaulting
    // this to 64 flattened the opening's cpr-96..127 SFX down onto the BGM and
    // let the mixer steal between them at random.
    int cpr;
    // The ARM7 player's PAUSE byte, +1 (PlayerInit clears it at 0x037FD9C4).
    // Command 0x02 writes it (sd_seq_pause); while it is set SND_SeqMain skips
    // PlayerSeqMain for this player and still runs PlayerUpdateChannel.
    int paused;
    // 1 while a sound mod's recording stands in for this sequence (lane
    // AUDIOHQ1): the sequence below runs exactly as before, silently, and the
    // mixer's stream voice for this player plays the recording.
    int replaced;
    sd_s16 var[SD_VARS];
    // SAVE-STATE MUSIC POSITION (lane TRANS093): how many sequencer ticks this
    // player has stepped since sd_seq_start, and the SSEQ entry it started
    // from. F8 records both, F9 starts the song again and sd_seq_seek runs it
    // forward silently to the recorded tick.
    sd_u32 tickAge;
    sd_u32 startOff;
    sd_u32 sig;             // hash of the 32 bytes at the entry: names the song across loads
};

Player g_pl[SD_PLAYERS];
// 1 while sd_seq_seek replays ticks: start_note makes no sound, so the track
// code runs exactly as it did the first time and only the voices are skipped.
int g_seeking;
sd_u32 g_seekPending[SD_PLAYERS];
// Positions F9 is waiting to apply: the restore asks the game for the song and
// the player that plays it can start a tick or two later (the command ring is
// drained on the next sound tick), so the request waits here for a matching
// start and goes stale after a few seconds.
SdSeqPos g_want[SD_PLAYERS];
int g_wantLeft;

/* TrackInit's defaults for a track opened by 0x93 on a sequence that did not
   pre-allocate it (and the same values sd_seq_start gives track 0 and the
   0xFE-allocated tracks). */
static void track_init_open(Track &t2, sd_u32 off)
{
    memset(&t2, 0, sizeof t2);
    t2.active = 1;
    t2.pc = off;
    t2.volume = 127; t2.expression = 127; t2.pan = 64;
    // Opened tracks start at the player's base priority, the same
    // as track 0; a 0xC6 in the opened track overrides it.
    /* Same TrackInit default as track 0; 0xC6 overrides it. */
    t2.bendRange = 2; t2.priority = 64; t2.noteWait = 1;
    t2.portaKey = 60;       /* TrackInit, 0x037FDB28 */
    t2.modSpeed = 16; t2.modRange = 1;      /* 0x037FBFBC */
    t2.envAttack = t2.envDecay = 0xff;      /* 0x037FDB0C */
    t2.envSustain = t2.envRelease = 0xff;
    t2.prog = 0;
}

// A mixer channel while a TRACK owns it. This is SND_seq.c's channelLLHead
// list turned inside out: the list is per track there, and here the ownership
// is recorded against the channel, which is the same relation read the other
// way round.
//
// OWNERSHIP OUTLIVES THE RELEASE, and that is the point. The ROM keeps a
// channel on its track's list from SND_AllocExChannel until the channel
// manager is finished with it and calls ChannelCallback with status 1
// (SND_seq.c line 653), which is well after the note was released -- the
// release tail is still the track's channel. noteFinishWait reads exactly
// that: it blocks the track while it still owns ANY channel.
//
// So `active` means OWNED, not sounding, and `released` says whether the
// note-off has already been sent. reap_finished_channels is the callback.
struct NoteSlot {
    int active;             // this channel belongs to (player, track)
    int released;           // the note-off has been sent; the tail is running
    int player, track;
    // -1 means "no scheduled note-off", and it is ONE meaning, not two.
    // SND_seq.c line 929 passes (length > 0) ? length : -1, so a duration-0
    // note and a tied note get the same lifetime; the track is what ends
    // either of them. Splitting them was this file's bug, not the ROM's.
    int ticks;
    int basePan;            // pan before the player's own bias
    int baseDb10;           // volume before the player's own attenuation
    int velocity;           // ch+0x09, the note's own 0..127 level
    // Playback rate for (key - baseNote) ALONE -- the ROM's
    // (chn->midiKey - chn->rootMidiKey) * 0x40 term and nothing else. Every
    // other pitch input the track has is re-applied to this every frame by
    // player_update_channels, so retuning a sounding voice starts from the
    // rate the note was born with rather than compounding the last offset
    // into the next one.
    double baseRate;
    // The track pitch last pushed onto this channel, so the dump can print a
    // line when the pitch MOVES rather than 192 identical lines a second.
    int lastUnits;
};
NoteSlot g_note[SD_CHANNELS];

unsigned g_rand = 0x12345678u;
int rnd(int lo, int hi)
{
    g_rand = g_rand * 1103515245u + 12345u;
    int span = hi - lo + 1;
    if (span <= 0) return lo;
    return lo + (int)((g_rand >> 16) % (unsigned)span);
}

inline sd_u8 rd8(const sd_u8 *p) { return *p; }
inline sd_s16 rd16s(const sd_u8 *p) { return (sd_s16)(p[0] | (p[1] << 8)); }
inline sd_u32 rd24(const sd_u8 *p)
{ return (sd_u32)p[0] | ((sd_u32)p[1] << 8) | ((sd_u32)p[2] << 16); }

sd_u32 read_varlen(const sd_u8 *seq, sd_u32 &pc)
{
    sd_u32 v = 0;
    for (int i = 0; i < 4; i++) {
        sd_u8 b = seq[pc++];
        v = (v << 7) | (b & 0x7f);
        if (!(b & 0x80)) break;
    }
    return v;
}

// SND_ReleaseExChannel: send the note-off and let the tail run. The channel
// STAYS the track's until it actually ends -- see NoteSlot.
void note_release(int ch, const char *why)
{
    if (ch < 0 || ch >= SD_CHANNELS) return;
    if (g_note[ch].released) return;
    g_note[ch].released = 1;
    // The pitch dump needs the release because the ROM stops updating a
    // channel's pitch here (envStatus 3 -- see retune_note_pitch), so a
    // reader diffing the two has to know where the trajectory legitimately
    // stops moving.
    SD_PD("rel f=%d p=%d t=%d ch=%d rate=%d ticks=%d why=%s\n", g_frame,
          g_note[ch].player, g_note[ch].track, ch, sd_mix_release_rate(ch),
          sd_mix_release_ticks(ch), why);
    sd_mix_release(ch, why);
}

// TrackFreeChannels: the track lets go of the channel. The sound is not
// stopped by this -- a released channel finishes its tail on its own, and the
// ROM's TrackStop is exactly TrackReleaseChannels followed by this.
void note_detach(int ch)
{
    if (ch < 0 || ch >= SD_CHANNELS) return;
    g_note[ch].active = 0;
}

// ChannelCallback with status 1 (SND_seq.c line 653): the channel manager has
// finished with the channel, so it comes off its track's list. The port has no
// callback out of the mixer, so the sequencer polls instead -- once per
// sequencer tick, sixteen tests. Without this nothing ever un-owns a channel
// and noteFinishWait would block its track forever.
void reap_finished_channels(void)
{
    for (int i = 0; i < SD_CHANNELS; i++)
        if (g_note[i].active && !sd_mix_active(i)) {
            SD_PD("end f=%d p=%d t=%d ch=%d\n", g_frame, g_note[i].player,
                  g_note[i].track, i);
            g_note[i].active = 0;
        }
}

/* THE TRACK'S PITCH, AND IT IS A PER-FRAME QUANTITY.
 *
 * SND_seq.c's TrackUpdateChannel, verbatim (lines 690 to 693):
 *
 *     pitch  = track->pitchBend;
 *     pitch *= track->bendRange << 6;
 *     pitch >>= 7;
 *     pitch += track->extPitch;
 *
 * and three lines further down it is STORED INTO EVERY CHANNEL THE TRACK OWNS:
 *
 *     for (chn = track->channelLLHead; chn; chn = chn->channelLLNext) {
 *         ...
 *         if (chn->envStatus == 3) continue;      // SND_ENV_RELEASE
 *         chn->userPitch = (s16)pitch;
 *
 * TrackUpdateChannel runs once per 192 Hz frame for every track of every
 * playing player (SND_SeqMain calls PlayerSeqMain then PlayerUpdateChannel),
 * and SND_ExChannelMain then computes the channel's timer from
 *
 *     pitch = (chn->midiKey - chn->rootMidiKey) * 0x40 + sweep
 *           + chn->userPitch + lfo
 *
 * So the bend is NOT part of the note. It rides on top of the note's own
 * interval, it is recomputed every frame, and a PITCHBEND written while a note
 * is still sounding bends THAT note.
 *
 * This file used to fold the bend into the note's rate once, at the note-on,
 * and never look at it again. Every song whose composer bends a note that is
 * already ringing was wrong for as long as that note lasted -- 19 of the EU
 * SDAT's 83 sequences, measured against a transliteration of the driver above.
 * The worst of them is the slide theme, NCS_BGM_ATHRETIC (SEQ 65, the BGM the
 * level table gives Cool Cool Mountain's slide, Tall Tall Mountain's slide and
 * the secret-slide courses): its lead track opens notes under PITCHBEND -11 at
 * BENDRANGE 12 and lets the bend go while they ring, so 684 of the lead's 3836
 * sounding frames -- 17.8%, 3.56 s out of a 20 s sample -- came out exactly
 * 103.1 cents flat. A semitone, on the melody. NCS_BGM_CHIJOU and
 * NCS_BGM_SHIRO, whose composers only bend between notes, measured 0.00%.
 *
 * THE UNITS ARE 1/64 OF A SEMITONE, 768 to the octave, and the s16 truncation
 * is the ROM's own: chn->userPitch is an s16 (SND_exChannel_shared.h +0x0E),
 * so a caller that manages to overflow it wraps on hardware too.
 *
 * >> 7 rather than / 128 for the same reason: the ROM's is an arithmetic
 * shift, which floors, and the two disagree by one unit on every negative
 * value that is not an exact multiple of 128.
 *
 * AND THE 7 IS NOT A 6. It is what makes a full-scale bend come out at exactly
 * +-bendRange semitones -- pitchBend is an s8, so full scale is +-128, and
 * bend * (bendRange << 6) >> 7 is bend * bendRange / 2 units, which is
 * bendRange semitones at 64 units to the semitone. This file once shifted by
 * one place less and every bend in the game was twice as deep as the DS plays
 * it: the ground loop re-randomises its bend with bendRange 2, so walking
 * warbled +-1.5 semitones instead of +-0.75.
 */
int track_pitch_units(const Track &tk)
{
    int pitch = tk.bend;
    pitch *= tk.bendRange << 6;
    pitch >>= 7;
    pitch += tk.pitch;          // extPitch, TRACK_PARAM 0x0c
    return (int)(sd_s16)pitch;
}

// 1/64 of a semitone as a playback-rate multiplier.
double pitch_units_scale(int units)
{
    return pow(2.0, (double)units / 64.0 / 12.0);
}

/* THE SWEEP A NOTE STARTS WITH, SM64DS's note-on at 0x037FD5B8..0x037FD61C.
 *
 *   sweepPitch  = track sweep (0xE3), plus (portaKey - key) * 64 when
 *                 portamento is on, stored back as an s16
 *   sweepLength = portaTime == 0 ? the note's length in 192 Hz frames
 *                                : (portaTime^2 * |sweepPitch|) >> 11
 *
 * and the channel then counts it down itself (mixer.cpp, 0x037FBC34). The
 * note's length in frames is the note-on's own conversion at 0x037FD42C:
 * (ticks * 240 + rate - 1) / rate with rate = tempo * tempoRatio >> 8, and
 * the port's tempoRatio is the ROM's default 256, so rate is the tempo. A
 * duration-0 note converts to 0 frames, so with portaTime 0 it does not
 * sweep at all; that is the ROM's arithmetic too.
 *
 * WHAT THE PORT DID BEFORE: parsed 0xC9 / 0xCE / 0xCF / 0xE3 and dropped
 * them, so every portamento and every sweep in the game played as a flat
 * note at its target pitch. 424 SEQARC entries set a portamento key. */
/* A note's own level before the two ext faders: the velocity term the
 * channel update adds (0x037FC5E0) plus the track update's volume +
 * expression + player volume (0x037FD694..0x037FD6B0), all through the
 * ARM7's table. Computed at the note-on AND every frame after it while the
 * track owns the channel -- see player_update_channels. */
int note_level_db10(const Player &pl, const Track &tk, int vel)
{
    int db10 = sd_cnv_vol(vel) + sd_cnv_vol(tk.volume)
             + sd_cnv_vol(tk.expression) + sd_cnv_vol(pl.volume);
    return db10 < -723 ? -723 : db10;
}

/* The track's envelope override onto the channel it just started or tied
 * onto, each field only when it is not 0xFF (0x037FD568..0x037FD5B4).
 *
 * WHAT THE PORT DID BEFORE: parsed 0xD0..0xD3 and dropped them, so every
 * voice ran its INSTRUMENT's envelope. 454 SEQARC entries set one, and the
 * ones the forum named are exactly this shape: the red coin (SSAR 2 entries
 * 47..55) asks for sustain 0 and decay 124, the stair timer's tick (SSAR 2
 * 56 / 57) for sustain 0 and decay 117 -- short blips that die away while
 * the note is still held -- and without the override they held at the
 * instrument's sustain for the note's whole length and then tailed off at
 * the instrument's release. */
void note_env(const Track &tk, int ch)
{
    if (tk.envAttack != 0xff || tk.envDecay != 0xff || tk.envSustain != 0xff
        || tk.envRelease != 0xff)
        sd_mix_set_env(ch, tk.envAttack == 0xff ? -1 : tk.envAttack,
                       tk.envDecay == 0xff ? -1 : tk.envDecay,
                       tk.envSustain == 0xff ? -1 : tk.envSustain,
                       tk.envRelease == 0xff ? -1 : tk.envRelease);
}

void note_sweep(const Player &pl, const Track &tk, int ch, int key, int ticks)
{
    int sweep = tk.sweepPitch;
    if (tk.portaOn) sweep += (sd_s16)((tk.portaKey - key) << 6);
    sweep = (sd_s16)sweep;
    int length;
    if (tk.portaTime == 0) {
        const int rate = pl.tempo;
        length = rate > 0 ? (ticks * 240 + rate - 1) / rate : 0;
    } else {
        const int mag = sweep < 0 ? -sweep : sweep;
        length = (tk.portaTime * tk.portaTime * mag) >> 11;
    }
    sd_mix_set_sweep(ch, sweep, length);
}

/* The channel this track owns, or -1. The ARM7 keeps a real linked list --
   head at track+0x3C, links through channel+0x50, prepended at 0x037FD55C --
   and this port scans instead, which finds the same channel because a track
   only ever holds one in the case that asks (TIE). */
int track_first_channel(int p, int t)
{
    for (int i = 0; i < SD_CHANNELS; i++)
        if (g_note[i].active && g_note[i].player == p && g_note[i].track == t)
            return i;
    return -1;
}

int track_has_channel(int p, int t)
{
    for (int i = 0; i < SD_CHANNELS; i++)
        if (g_note[i].active && g_note[i].player == p && g_note[i].track == t)
            return 1;
    return 0;
}

// Kill everything a player owns.
void player_silence(int p)
{
    for (int i = 0; i < SD_CHANNELS; i++)
        if (g_note[i].active && g_note[i].player == p) {
            note_release(i, "player stopped out from under the note");
            note_detach(i);
        }
}

// TrackStop: TrackReleaseChannels then TrackFreeChannels (SND_seq.c line 618).
// Every channel the track owns, with no test of any kind on the note -- a tied
// note and a duration-0 note are released by the same line, because they are
// the same thing to the ROM.
//
// This is where a track ENDS, and every way out of a track comes through here:
// the 0xff the stream asked for (PlayerStopTrack on TrackMain's -1, line 1268)
// and the abnormal deaths below. A track that dies from a desynced stream has
// to take its voices with it or the garbage note it keyed on the way out rings
// forever, which is what the loud static in the 2026-08-05 play session was.
void track_stop(int p, int t, const char *why)
{
    for (int i = 0; i < SD_CHANNELS; i++)
        if (g_note[i].active && g_note[i].player == p && g_note[i].track == t) {
            note_release(i, why);
            note_detach(i);
        }
}

// THE COIN. SEQARC 3 entry 0x11 is the chime Actor::GivePlayerCoins plays on
// every coin:
//
//     PROG 19 ... TRANSPOSE 24
//     NOTE 46 dur 2 / NOTE 58 dur 7 / NOTE 51 dur 5
//     TIE 1
//     NOTE 63 dur 25
//     PRIO 16
//     NOTE 63 dur 30
//     END
//
// TIE is set and never cleared, so its last two notes are started with no
// scheduled note-off, and before track_stop ran at 0xff the end of the track
// left them sounding. In the castle banks program 19 is a one-shot sample, so
// they ran out by themselves and nothing showed. In every COURSE bank (32..55,
// and Bob-omb Battlefield is 0x21) program 19 is a LOOPING SWAV -- 15024
// samples with its loop point at 7544 -- and a looping sample that is never
// released does not end. It sat at priority 64 until some later sound happened
// to steal its channel, which is why it sounded random rather than constant:
// quiet spot, it droned; busy spot, it was gone at once.
//
// Note what does NOT fix it: releasing only the tied notes. The durations are
// nonzero here, so this chime reaches its 0xff either way -- but SEQARC 2 entry
// 231 is PROG 17 / TIE 1 / NOTE 60 dur 0 / END on a looping SWAV, an intended
// sustained drone, and it NEVER reaches its 0xff because noteFinishWait parks
// the track on a channel that never ends. The two are told apart by that, not
// by the tie.

/* THE PSG VOICES. A type 2 note is the DS's pulse generator and a type 3 note
 * its noise generator; neither has a wave in the archive. The channel start
 * (0x037FE3C4) gives both the same base timer, 0x1F46 = 8006 (0x037FC468 /
 * 0x037FC4BC, stored at ch+0x3C), so at the record's root key a hardware step
 * comes every 8006 ticks of the 16.757 MHz clock: 2093 steps a second (the
 * port's integer rate; the exact 2093.05 is 0.003 percent away). A pulse is 8
 * steps to a cycle, so the root key sounds at 261.6 Hz, which is C4.
 *
 * PULSE: the record's first u16 is the duty, 0..6 = 1..7 high steps of 8, 7 =
 * silent (GBATEK, "DS sound channels", and the same table in melonDS's SPU);
 * the high steps are the last ones of the cycle, and the level is +-0x7FFF,
 * a 16 bit sample at full scale that then runs the same volume path as a
 * sampled voice.
 *
 * NOISE: the 15 bit shift register the hardware clocks once per step: start
 * 0x7FFF, shift right, and when the bit that fell out was 1 xor 0x6000 in and
 * output -0x7FFF, otherwise +0x7FFF. Its period is 32767 steps, so the whole
 * cycle is one looping table.
 *
 * Both play as a looping table at the pulse's own rate with no interpolation
 * (sd_mix_set_hold). WHAT THE PORT DID BEFORE: read the duty as an index into
 * the linked SWAR and played that wave, so the red coin's jingle (SEQARC 2
 * entries 47..55, program 128 of bank 1, a duty 3 pulse) and Balloon Mario's
 * B (SEQARC 0 entry 190, program 107 of bank 0, also duty 3) sounded as a
 * 2976 sample ADPCM click, and so does every other pulse instrument. */
const sd_s16 *psg_table(const SdatNote &n, SdatWave *w)
{
    static sd_s16 pulse[8][8];
    static sd_s16 noise[32767];
    static int built;
    if (!built) {
        built = 1;
        for (int d = 0; d < 8; d++)
            for (int k = 0; k < 8; k++)
                pulse[d][k] = (d < 7 && k >= 7 - d) ? 0x7fff : -0x7fff;
        unsigned lfsr = 0x7fff;
        for (int k = 0; k < 32767; k++) {
            const unsigned out = lfsr & 1;
            lfsr >>= 1;
            if (out) lfsr ^= 0x6000;
            noise[k] = out ? -0x7fff : 0x7fff;
        }
    }
    w->loop = 1;
    w->loopStart = 0;
    w->sampleRate = 2093;
    if (n.type == 3) {
        w->totalSamples = 32767;
        return noise;
    }
    w->totalSamples = 8;
    return pulse[n.swav & 7];
}

void start_note(Player &pl, int pi, int ti, Track &tk, int note, int vel,
                int ticks)
{
    if (g_seeking) return;
    if (!pl.sbnk) {
        SD_VT("note p%d t%d key %d DROPPED: player has no bank\n", pi, ti,
              note);
        return;
    }
    SdatNote n;
    const sd_u8 *swar = 0;
    int key = note + tk.transpose;
    if (key < 0) key = 0;
    if (key > 127) key = 127;
    if (!sdat_bank_note(pl.sbnk, tk.prog, key, &n, &swar)) {
        SD_VT("note p%d t%d key %d DROPPED: program %d has no note there\n",
              pi, ti, key, tk.prog);
        return;
    }

    SdatWave w;
    w.ownPitch = 0;
    const int psg = n.type == 2 || n.type == 3;
    if (psg) {
        w.pcm = psg_table(n, &w);
    } else if (!sdat_swar_wave(swar, n.swav, &w)) {
        SD_VT("note p%d t%d key %d DROPPED: wave %d unresolvable\n", pi, ti,
              key, n.swav);
        return;
    }

    int baseDb10 = note_level_db10(pl, tk, vel);
    int db10 = baseDb10 + pl.volDb10 + tk.volDb10;
    if (db10 < -723) db10 = -723;

    int basePan = tk.panSet ? tk.pan : n.pan;
    // The player's own pan biases the track's, centred at 64.
    int pan = basePan + (pl.pan - 64);
    if (pan < 0) pan = 0;
    if (pan > 127) pan = 127;

    /* The note's OWN interval, and nothing else: the ROM's
       (chn->midiKey - chn->rootMidiKey) * 0x40, in semitones. This is the one
       pitch input that is fixed for the life of the note, so it is the one
       that belongs in baseRate.

       EVERYTHING THE TRACK CONTRIBUTES IS KEPT OUT OF IT ON PURPOSE. The pitch
       bend used to be folded in here, which made it a property of the note;
       on hardware it is a property of the TRACK, recomputed and re-applied
       every frame, and a bend written while the note rings bends the ringing
       note. TRACK_PARAM 0x0c was already outside for that reason and the bend
       now joins it -- one quantity, one place, track_pitch_units. */
    double semis = (double)(key - n.baseNote);
    double baseRate = (double)w.sampleRate * pow(2.0, semis / 12.0)
                    / sd_mix_rate();
    int pitchUnits = track_pitch_units(tk);
    double rate = baseRate * pitch_units_scale(pitchUnits);
    if (w.ownPitch) {
        /* A wave a mod replaced with its own recording plays at the file's
           natural pitch: no key offset, bend, sweep, portamento or modulation
           reaches it. Volume, pan, envelope and stop still do. */
        semis = 0.0;
        baseRate = (double)w.sampleRate / sd_mix_rate();
        pitchUnits = 0;
        rate = baseRate;
    }
    // Everything that can refuse the note is settled BEFORE a channel is
    // taken. Allocating first and then bailing on the rate left a stolen
    // channel dead with the previous note's owner still recorded against it,
    // which is a voice lost for nothing.
    if (rate <= 0.0 || rate > 64.0) {
        SD_VT("note p%d t%d key %d DROPPED: playback rate %.3f out of "
              "range\n", pi, ti, key, rate);
        SD_PD("off f=%d p=%d t=%d ch=-1 key=%d prio=%d why=rate rate=%.9f\n",
              g_frame, pi, ti, key, pl.cpr + tk.priority, rate);
        return;
    }
    /* The inputs the rate is made of. UNCONDITIONAL, one line per note-on.
       This used to fire only when the note landed more than a semitone from
       the wave's own pitch, on the theory that a note sitting at its base
       pitch has nothing interesting to say about pitch. That was the wrong
       trade: it also meant a track playing near its base pitch produced NO
       success record at all, so pairing survivors against the DROPPED lines
       below -- "did that part go silent, or just thin out" -- could not be
       answered from this trace. A dropped note only means something next to
       the notes that were NOT dropped, so both sides have to be printed. */
    SD_VT("note p%d t%d key %d: %+.2f semitones off base %d "
          "(transpose %d, bend %d, range %d, track pitch %d units) -> "
          "rate %.4f\n",
          pi, ti, key, semis, n.baseNote, tk.transpose, tk.bend,
          tk.bendRange, pitchUnits, rate);

    /* THE ALLOCATOR PRIORITY IS THE SUM OF THE PLAYER'S AND THE TRACK'S.
       ARM7 note-on at 0x037FD410:
           ldrb r2, [r7, #4]    ; player->priority  (the SDAT cpr)
           ldrb r1, [r8, #0x12] ; track->priority   (64, or the 0xC6 value)
           add  r1, r2, r1      ; <<< the sum is what AllocChannel gets
       Both addends are byte loads and the sum is NOT clamped, so it runs
       0..510 even though the channel stores it back through a strb. With
       this ROM's values it never exceeds 255, but the accept/reject test
       upstream uses the untruncated sum, so the sum is what we pass.

       WHAT THIS ACTUALLY COMES TO, MEASURED, IN THE OPENING. cpr is not one
       value across the game: of the 83 SEQ records, 37 carry cpr 64 and 32
       carry 106. THE OPENING'S BGM IS ONE OF THE 64s. Its tracks set 0xC6 in
       the 64..68 band, so it asks the allocator for 128..132, and that is
       exactly what the dropped-note trace shows. The sound effects over it
       come out of the SEQARCs at cpr 96 with no 0xC6 at all, so they ask 160.
       So in THIS scene the effects outrank the music, and the ROM's own
       arithmetic says they should.

       (An earlier version of this comment claimed the reverse, on the 106
       sequences. Those exist, and for them the music does outrank a 96 effect
       -- but they are not the opening, and the general claim was wrong. See
       sdat.cpp, which has the same numbers from the other side.)

       Folding cpr into the TRACK priority, as this port briefly did, is wrong
       for a different reason than the ordering: 0xC6 then overwrites it, so
       the player's contribution disappears entirely rather than being added. */
    /* A TIED NOTE REUSES THE TRACK'S CHANNEL. IT DOES NOT OPEN ANOTHER.
       ARM7 note-on, 0x037FD454: it tests track flags bit 3 (tie) and, if the
       track's channel list is non-empty, writes the new key and velocity into
       the channel it already owns (ch+0x08, ch+0x09) and branches PAST the
       allocation entirely -- "cmp r4,#0 / bne 0x37fd568". The per-frame walker
       at 0x037FD678 then pushes the new pitch and volume down. One tied track
       therefore costs exactly one channel no matter how many notes it plays.

       THIS IS THE VOICE LEAK. Without it a tied track opens a fresh channel per
       note and never closes any, because a tied note also gets no scheduled
       note-off (length is forced to 0 at 0x037FD50C). In the opening, player 11
       track 0 is such a track: it fired a note about every 0.4 s and by 25 s
       held FIFTEEN of the sixteen channels, all still sounding, the oldest
       since 18.9 s. The music, asking at 129 to 131 against those voices at
       160, was refused every time for about six seconds. The ROM holds one
       channel there.

       Note this is NOT the same fix as the release-priority collapse in
       sd_mix_release: that one governs a voice whose track has ENDED, this one
       stops the voices being created in the first place. Both were needed. */
    if (tk.tie) {
        const int held = track_first_channel(pi, ti);
        if (held >= 0) {
            g_note[held].basePan   = basePan;
            g_note[held].baseDb10  = baseDb10;
            g_note[held].velocity  = vel;
            g_note[held].baseRate  = baseRate;
            g_note[held].lastUnits = pitchUnits;
            g_note[held].ticks     = -1;   /* tie: no scheduled note-off */
            g_note[held].released  = 0;
            sd_mix_set(held, db10, pan, rate);
            sd_mix_set_pitch_base(held, w.ownPitch ? -1.0 : baseRate, pitchUnits);
            note_env(tk, held);
            note_sweep(pl, tk, held, key, ticks);
            SD_VT("note p%d t%d key %d TIED onto chan %d (no new voice)\n",
                  pi, ti, key, held);
            SD_PD("tie f=%d p=%d t=%d ch=%d key=%d\n", g_frame, pi, ti, held,
                  key);
            return;
        }
    }

    const int prio = pl.cpr + tk.priority;
    /* The channel set follows the note type: 0xFFFF sampled, 0x3F00 pulse,
       0xC000 noise (0x037FD4A0..0x037FD4D0). */
    int ch = sd_mix_alloc_mask(prio, n.type == 2 ? 0x3f00u
                                     : n.type == 3 ? 0xc000u : 0xffffu);
    if (ch < 0) {
        SD_VT("note p%d t%d key %d DROPPED: no mixer channel free\n", pi, ti,
              key);
        SD_PD("off f=%d p=%d t=%d ch=-1 key=%d prio=%d why=nochan\n",
              g_frame, pi, ti, key, prio);
        return;
    }
    if (g_note[ch].active)
        SD_VT("chan %2d taken from player %d track %d\n", ch,
              g_note[ch].player, g_note[ch].track);

    sd_mix_start(ch, &w, &n, db10, pan, rate, prio);
    if (psg) sd_mix_set_hold(ch);
    sd_mix_set_pitch_base(ch, w.ownPitch ? -1.0 : baseRate, pitchUnits);
    if (pl.replaced) sd_mix_set_mute(ch);       // the recording plays instead
    note_env(tk, ch);
    note_sweep(pl, tk, ch, key, ticks);
    g_note[ch].active = 1;
    g_note[ch].player = pi;
    g_note[ch].track = ti;
    g_note[ch].basePan = basePan;
    g_note[ch].baseDb10 = baseDb10;
    g_note[ch].velocity = vel;
    g_note[ch].baseRate = baseRate;
    g_note[ch].lastUnits = pitchUnits;
    // The numeric dump. Every input the DS's own pitch is made of, in the
    // DS's own units, and the channel it landed on, so an off-key report can
    // be answered with a per-voice diff against the driver rather than with
    // an ear. See sd_pdump.
    SD_PD("on f=%d p=%d t=%d ch=%d prog=%d note=%d transpose=%d key=%d "
          "root=%d bend=%d range=%d ext=%d pu=%d srate=%u rate=%.9f\n",
          g_frame, pi, ti, ch, tk.prog, note, tk.transpose, key,
          (int)n.baseNote, tk.bend, tk.bendRange, tk.pitch, pitchUnits,
          (unsigned)w.sampleRate, rate);
    // Duration 0 means "no scheduled note-off" -- the note runs until its
    // envelope or its sample ends. Every sound effect in the SEQARCs is
    // written that way (a lone "program change, note, end of track"), so
    // treating 0 as a one-tick note cut all of them to a click.
    //
    // SND_seq.c line 929: (length > 0) ? length : -1, and SND_NoteOn is handed
    // -1 outright when the track is tied. Duration 0 and TIE reach the same
    // place. -1 is "no scheduled note-off"; the TRACK ends the note, at its
    // 0xff (track_stop) or by being blocked on it (noteFinishWait).
    g_note[ch].ticks    = (tk.tie || ticks <= 0) ? -1 : ticks;
    g_note[ch].released = 0;
}

// Execute one track until it must wait. Returns 0 if the track ended.
int run_track(Player &pl, int pi, int ti)
{
    Track &tk = pl.tr[ti];
    const sd_u8 *s = pl.seq;

    for (int guard = 0; guard < 4096; guard++) {
        if (!tk.active) return 0;
        // SND_seq.c lines 867 to 871, and it comes BEFORE the tick wait for
        // the same reason it does there: a track parked on a duration-0 note
        // is waiting on its CHANNELS, not on a count. It comes back to life
        // the tick after reap_finished_channels takes its last one away.
        if (tk.noteFinishWait) {
            if (track_has_channel(pi, ti)) return 1;
            tk.noteFinishWait = 0;
        }
        if (tk.wait > 0) return 1;

        // Prefix state for 0xA0/0xA1/0xA2.
        int useRandom = 0, useVar = 0, condition = 1;
        sd_u8 op;
        for (;;) {
            op = s[tk.pc++];
            if (op == 0xa0) { useRandom = 1; continue; }
            if (op == 0xa1) { useVar = 1; continue; }
            if (op == 0xa2) { condition = tk.cmpFlag; continue; }
            break;
        }

        // Helper lambdas read the "last argument" honouring the prefixes.
        auto argU8 = [&](void) -> int {
            if (useVar) { int v = pl.var[s[tk.pc++] & (SD_VARS - 1)]; return v; }
            if (useRandom) {
                int lo = rd16s(s + tk.pc); tk.pc += 2;
                int hi = rd16s(s + tk.pc); tk.pc += 2;
                return rnd(lo, hi);
            }
            return s[tk.pc++];
        };
        auto argS16 = [&](void) -> int {
            if (useVar) { int v = pl.var[s[tk.pc++] & (SD_VARS - 1)]; return v; }
            if (useRandom) {
                int lo = rd16s(s + tk.pc); tk.pc += 2;
                int hi = rd16s(s + tk.pc); tk.pc += 2;
                return rnd(lo, hi);
            }
            int v = rd16s(s + tk.pc); tk.pc += 2; return v;
        };
        // The VARLEN arguments (note duration, rest, program) take the
        // prefixes too. A 0xA0-prefixed rest stores lo,hi as two s16s where
        // the varlen would sit, and reading them as a varlen would walk the
        // stream off by the difference. Spec correctness, not a live bug: a
        // static walk of every SSEQ and SEQARC entry in the EU SDAT (calls
        // and prefixes modelled) found no 0xA0 in front of a varlen anywhere,
        // so the 2026-08-05 pc-6664 track death was NOT this -- see the
        // unimplemented-opcode handler below for where that hunt points.
        auto argVarlen = [&](void) -> sd_u32 {
            if (useVar) {
                int v = pl.var[s[tk.pc++] & (SD_VARS - 1)];
                return (sd_u32)(v < 0 ? 0 : v);
            }
            if (useRandom) {
                int lo = rd16s(s + tk.pc); tk.pc += 2;
                int hi = rd16s(s + tk.pc); tk.pc += 2;
                int v = rnd(lo, hi);
                return (sd_u32)(v < 0 ? 0 : v);
            }
            return read_varlen(s, tk.pc);
        };

        if (op < 0x80) {                        // note on
            int vel = s[tk.pc++];
            sd_u32 dur = argVarlen();
            if (condition) start_note(pl, pi, ti, tk, op, vel, (int)dur);
            /* 0x037FCCC0: every note that runs leaves its key as the next
               portamento start, whether or not it found a voice. */
            if (condition) {
                int k = op + tk.transpose;
                tk.portaKey = k < 0 ? 0 : (k > 127 ? 127 : k);
            }
            // SND_seq.c lines 936 to 939. Duration 0 does not mean "wait no
            // time", it means "wait for the note itself", and that is what
            // holds a one-shot's track open until its sample has finished.
            if (condition && tk.noteWait) {
                tk.wait = (int)dur;
                if (dur == 0) tk.noteFinishWait = 1;
            }
            continue;
        }

        switch (op) {
        case 0x80: {                            // rest
            sd_u32 d = argVarlen();
            if (condition) tk.wait = (int)d;
            break;
        }
        case 0x81: {                            // program change
            // NOT `& 0x7f`. That mask is the general-MIDI habit and it is
            // wrong for THIS SDAT, which the data says plainly:
            //
            //   * the SBNKs are bigger than 128 instruments. Slot counts in
            //     data/sound_data.sdat run 209, 210, 212, 221, 301 and 491,
            //     and the instrument records past 127 are real -- bank 0x36
            //     (NCS_BANK_SE_T_VS, the one the VS arenas load) carries
            //     instruments at 131, 200 and 209. Under a 7-bit mask no
            //     sequence can ever address them, so they would be dead bytes
            //     in a shipped ROM.
            //   * the sequences encode exactly those numbers. Take
            //     NCS_SE_SCT_GLASS_BROKEN, the star container shattering. It
            //     is NOT a standalone SSEQ: it has no FAT entry and no 0x1c
            //     SSEQ header of its own. It is SEQARC 3 (NCS_SEQARC_SCENE)
            //     SUB-ENTRY 83, whose record gives a seqOffset of 0x105a from
            //     that SEQARC's data base, i.e. absolute sdat 0x4b8ee. (The
            //     FAT-entry-plus-0x1c-header route is the one seq 0x4d, the
            //     arena music, travels; do not mix them up.) Its whole body at
            //     that address is seven bytes, `81 81 48 3c 6e 00 ff`: a
            //     program change whose varlen is two bytes and works out to
            //     200, then one note, then end of track. Bank 0x36 has 200.
            //   * and the masked value is unreachable. 200 & 0x7f is 72, and
            //     program 72 is present in NOT ONE of the SDAT's 134 banks.
            //     A read that produces an index no bank in the game can
            //     answer is the wrong read, and start_note's own
            //     "program %d has no note there" is where it ended: the
            //     request arrived, the voice was allocated, and the note was
            //     dropped one step later, without a sound.
            //
            // ---- WHAT CHANGES, WHICH IS THE PART WORTH ARGUING ABOUT -------
            //
            // A conservative count -- only entries whose FIRST command is the
            // program change, so no jump or call is followed -- puts 141
            // SEQARC entries on the wrong side of that mask. Sorted by what a
            // player would actually notice, against the banks a stage can
            // load (the level table's own bank column over the 0x34 levels,
            // plus 0x36):
            //
            //   AUDIBLE CHANGE, up to 31 entries. Nineteen of them are
            //   SEQARC_SCENE entries with at least one stage bank where BOTH
            //   the masked and the unmasked program are real instruments, so
            //   the sound was never missing -- it was the WRONG INSTRUMENT,
            //   and it becomes the right one. That set includes
            //   NCS_SE_SCT_ENEMYDOWN (program 170, masked 42), which is the
            //   most-heard sound in this whole change, and the enemy family
            //   around it: DON_ATTACK, BDN_ATTACK, KRB_DOWN, NOK_CRY,
            //   JAN_ATTACK/DAMAGE, KMO_WALK, CHO_DOWN, KID_ATTACK,
            //   BMK_MESSAGE/DAMAGE, plus FIRE_GO_OUT, KTL_FIRE_GO_OUT,
            //   UKY_SCRATCH, RBM_MESSAGE, PUZZLE and FLOWER_BLOOM.
            //
            //   SILENT BEFORE, 69 entries (60 is the SEQARC_SCENE bucket
            //   alone, not the total -- an earlier draft of this comment
            //   quoted the bucket as the total). These had no instrument at
            //   the masked index in any stage bank, so they made no sound at
            //   all; GLASS_BROKEN is one of them. None is in the core
            //   movement loop, which is why the port walked this far without
            //   the mask being noticed.
            //
            //   ONE REGRESSION, and it is documented rather than warned
            //   about: NCS_SE_SCL_PEACH_STAR (program 139, masked 11) goes
            //   silent everywhere. That is not a defect in this read --
            //   program 139 is a HOLE in every stage bank, while 140 right
            //   beside it is filled in nearly all of them. The instrument the
            //   sound asks for is not in the ROM, so the DS's own lookup
            //   answers the same nothing. It was audible before only because
            //   the mask happened to land it on instrument 11.
            //
            // Net: 140 of 141 improve or are neutral.
            //
            // Requests do not move at all -- this sits downstream of every
            // decision Sound::Play makes. Measured over a five-level sweep
            // (1, 2, 6, 9, 14) the request streams are identical line for
            // line, and the voice-start deltas are +1, -3, +1, -2 and 0.
            //
            // sdat_bank_note already bounds the index against the bank's own
            // instrument count, so an out-of-range program still refuses --
            // it just refuses for the bank's reason instead of a made-up one.
            sd_u32 v = argVarlen();
            if (condition) tk.prog = (int)v;
            break;
        }
        case 0x93: {                            // open track
            int n = s[tk.pc++];
            sd_u32 off = rd24(s + tk.pc); tk.pc += 3;
            if (condition && n > 0 && n < SD_TRACKS && !pl.tr[n].active) {
                Track &t2 = pl.tr[n];
                if (t2.allocd) {
                    // Built at START by the 0xFE mask: just point it at its
                    // code, keep everything the game already wrote to it.
                    t2.allocd = 0;
                    t2.active = 1;
                    t2.pc = off;
                } else {
                    track_init_open(t2, off);
                }
            }
            break;
        }
        case 0x94:                              // jump
            { sd_u32 off = rd24(s + tk.pc); tk.pc += 3;
              if (condition) tk.pc = off; }
            break;
        case 0x95:                              // call
            { sd_u32 off = rd24(s + tk.pc); tk.pc += 3;
              if (condition && tk.sp < SD_STACK) {
                  tk.stack[tk.sp++] = tk.pc; tk.pc = off;
              } }
            break;
        case 0xfd:                              // return, or end of track
            // A return with an EMPTY call stack ends the track, exactly like
            // 0xff. 27 of the EU SDAT's SEQARC entries end with 0xfd rather
            // than 0xff, and treating it as a no-op walked the pc off the
            // end of the entry into the neighbouring entries and then out of
            // the archive: garbage notes tying up mixer channels until only
            // high-priority voices could still play, then a track death at
            // the first invalid byte. That was the whole static-then-silence
            // arc of the 2026-08-05 evening session; all four of its logged
            // track deaths started at legit SEQARC entries.
            if (condition) {
                if (tk.sp > 0) tk.pc = tk.stack[--tk.sp];
                else {
                    track_stop(pi, ti, "track ended (0xfd, no caller)");
                    tk.active = 0;
                    return 0;
                }
            }
            break;
        case 0xd4:                              // loop start
            { int n = argU8();
              if (condition && tk.loopSp < SD_STACK) {
                  tk.loopPc[tk.loopSp] = tk.pc;
                  tk.loopCount[tk.loopSp] = n;   // 0 == infinite
                  tk.loopSp++;
              } }
            break;
        case 0xfc:                              // loop end
            if (condition && tk.loopSp > 0) {
                int i = tk.loopSp - 1;
                if (tk.loopCount[i] == 0) { tk.pc = tk.loopPc[i]; }
                else if (--tk.loopCount[i] > 0) { tk.pc = tk.loopPc[i]; }
                else tk.loopSp--;
            }
            break;
        case 0xff:                              // end of track
            track_stop(pi, ti, "track ended (0xff)");
            tk.active = 0;
            return 0;
        case 0xfe:                              // alloc tracks (bitmask)
            tk.pc += 2;
            break;

        case 0xc0: { int v = argU8(); if (condition) { tk.pan = v; tk.panSet = 1; } break; }
        case 0xc1: { int v = argU8(); if (condition) tk.volume = v; break; }
        case 0xc2: { int v = argU8(); if (condition) pl.volume = v; break; }
        case 0xc3: { int v = (sd_s8)argU8(); if (condition) tk.transpose = v; break; }
        case 0xc4: { int v = argU8(); if (condition) tk.bend = (sd_s8)v; break; }
        case 0xc5: { int v = argU8(); if (condition) tk.bendRange = v; break; }
        case 0xc6: { int v = argU8(); if (condition) tk.priority = v; break; }
        case 0xc7: { int v = argU8(); if (condition) tk.noteWait = v; break; }
        /* 0xC8 (TIE) also RELEASES whatever the track is holding. The ARM7
           calls ReleaseTrackChannels(track, player, -1) from the opcode at
           0x037FCFFC, the same routine 0xFF goes through, so changing tie
           mode ends the current voices instead of stranding them. */
        case 0xc8: { int v = argU8();
            if (condition) { tk.tie = v; track_stop(pi, ti, "tie changed"); }
            break; }
        case 0xd5: { int v = argU8(); if (condition) tk.expression = v; break; }

        /* PORTAMENTO, SM64DS's opcode table at 0x037FCE94. 0xC9 sets the key
           the next note glides FROM (plus the track's transpose) and turns
           portamento on; 0xCE turns it on or off; 0xCF sets its time. 0xC9
           and 0xCE also release and free the track's voices -- both call
           0x037FD948, the routine 0xC8 and 0xFF go through -- so they end
           here the same way TIE does. */
        case 0xc9: { int v = argU8();
            if (condition) {
                tk.portaKey = (sd_u8)(v + tk.transpose);
                tk.portaOn = 1;
                track_stop(pi, ti, "portamento key set");
            }
            break; }
        case 0xce: { int v = argU8();
            if (condition) {
                tk.portaOn = v & 1;
                track_stop(pi, ti, "portamento switched");
            }
            break; }
        case 0xcf: { int v = argU8(); if (condition) tk.portaTime = v; break; }

        /* MODULATION, the same table: 0xCA depth (+0x1a), 0xCB speed
           (+0x19), 0xCC target (+0x18), 0xCD range (+0x1b), 0xE0 delay
           (+0x1c). */
        case 0xca: { int v = argU8(); if (condition) tk.modDepth = v & 0xff; break; }
        case 0xcb: { int v = argU8(); if (condition) tk.modSpeed = v & 0xff; break; }
        case 0xcc: { int v = argU8(); if (condition) tk.modTarget = v & 0xff; break; }
        case 0xcd: { int v = argU8(); if (condition) tk.modRange = v & 0xff; break; }
        case 0xe0:                              // modulation delay
            { int v = argS16(); if (condition) tk.modDelay = v & 0xffff; }
            break;

        /* THE ENVELOPE OVERRIDE, the same table: 0xD0 attack (+0x0e), 0xD1
           decay (+0x0f), 0xD2 sustain (+0x10), 0xD3 release (+0x11). */
        case 0xd0: { int v = argU8(); if (condition) tk.envAttack = v & 0xff; break; }
        case 0xd1: { int v = argU8(); if (condition) tk.envDecay = v & 0xff; break; }
        case 0xd2: { int v = argU8(); if (condition) tk.envSustain = v & 0xff; break; }
        case 0xd3: { int v = argU8(); if (condition) tk.envRelease = v & 0xff; break; }
        case 0xd6:                              // print variable: debug only
            argU8();
            break;
        case 0xe3:                              // sweep pitch, track +0x16
            { int v = argS16(); if (condition) tk.sweepPitch = (sd_s16)v; }
            break;
        case 0xe1:                              // tempo
            { int v = argS16(); if (condition && v > 0) pl.tempo = v; }
            break;

        default:
            if (op >= 0xb0 && op <= 0xbd) {     // variable ops
                int vn = s[tk.pc++] & (SD_VARS - 1);
                int v = argS16();
                sd_s16 &V = pl.var[vn];
                if (condition) switch (op) {
                case 0xb0: V = (sd_s16)v; break;
                case 0xb1: V = (sd_s16)(V + v); break;
                case 0xb2: V = (sd_s16)(V - v); break;
                case 0xb3: V = (sd_s16)(V * v); break;
                case 0xb4: if (v) V = (sd_s16)(V / v); break;
                case 0xb5: V = (sd_s16)(v >= 0 ? (V << v) : (V >> -v)); break;
                case 0xb6: V = (sd_s16)rnd(v < 0 ? v : 0, v < 0 ? 0 : v); break;
                case 0xb8: tk.cmpFlag = (V == v); break;
                case 0xb9: tk.cmpFlag = (V >= v); break;
                case 0xba: tk.cmpFlag = (V >  v); break;
                case 0xbb: tk.cmpFlag = (V <= v); break;
                case 0xbc: tk.cmpFlag = (V <  v); break;
                case 0xbd: tk.cmpFlag = (V != v); break;
                }
                break;
            }
            {
                static sd_u8 seen[256];
                if (!seen[op]) {
                    seen[op] = 1;
                    /* Every stream in the EU SDAT parses clean end to end
                       under this dispatcher (statically walked, calls and
                       prefixes modelled), so landing here means the pc or
                       the seq POINTER went somewhere no stream reaches --
                       state corruption, not a missing opcode. Name the
                       stream so the playlog says which one died: the
                       sdat-relative offset identifies it. */
                    long rel = (pl.seq >= g_sdat.base &&
                                pl.seq < g_sdat.base + g_sdat.size)
                               ? (long)(pl.seq - g_sdat.base) : -1;
                    fprintf(stderr, "[sseq] unimplemented opcode 0x%02x "
                            "(player %d track %d, pc %u, seq %p = "
                            "sdat+0x%lx) -- track ended; no stream parses "
                            "here, suspect a stomped pc or seq pointer\n",
                            op, pi, ti, (unsigned)(tk.pc - 1),
                            (const void *)pl.seq, rel);
                }
                track_stop(pi, ti, "track died on a desynced stream");
                tk.active = 0;
                return 0;
            }
        }
    }
    // A track that never yields is malformed; stop it rather than hang.
    fprintf(stderr, "[sseq] player %d track %d ran 4096 commands without a "
            "wait -- stopped\n", pi, ti);
    track_stop(pi, ti, "track ran away without a wait");
    tk.active = 0;
    return 0;
}

}  // namespace

void sd_seq_reset(void)
{
    memset(g_seekPending, 0, sizeof g_seekPending);
    memset(g_want, 0, sizeof g_want);
    g_wantLeft = 0;
    g_seeking = 0;
    // Latched here for sd_mix_reset's reason: this runs before any sound can,
    // and the flag has to be readable from the first note-on.
    pdump_open();
    memset(g_pl, 0, sizeof g_pl);
    memset(g_note, 0, sizeof g_note);
    g_frame = 0;
}

int sd_seq_active(int p)
{
    if (p < 0 || p >= SD_PLAYERS) return 0;
    if (!g_pl[p].active) return 0;
    for (int i = 0; i < SD_TRACKS; i++)
        if (g_pl[p].tr[i].active) return 1;
    return 0;
}

// SNDSharedWork.playerStatus: one bit per player, set while that player is
// still holding the sequence it was started with. `active` and not merely
// "a track is running" is the right test -- sd_seq_frame keeps the player
// marked active after its last track ends until the release tails finish,
// which is exactly the window the DS's own player occupies.
sd_u32 sd_seq_player_mask(void)
{
    sd_u32 m = 0;
    for (int p = 0; p < SD_PLAYERS; p++)
        if (g_pl[p].active) m |= 1u << p;
    return m;
}

// startOff is the entry's offset inside seqBase, and it stays SEPARATE from
// the base on purpose.
//
// Every pc in this player -- the initial one, every 0x94 jump, every 0x95
// call, every 0x93 open-track -- is an offset from seqBase. In a SEQARC that
// base is the archive's whole DATA block and the entry is somewhere inside
// it, so folding the entry offset into the base (seqBase = base + entry, pc
// = 0) reads correctly for exactly as long as the stream runs straight: the
// first branch then lands at base + entry + target instead of base + target,
// short by nothing on entry 0 and by the entry's own offset on every other
// one.
//
// What that sounds like is the second half of the sliding-effect report.
// SEQARC entry 67 at archive offset 0xd01 is a looping effect -- random
// detune, random volume, one note, rest, jump back to 0xd23 -- and folded,
// its jump landed 0xd01 further on, in the middle of another entry, on a
// byte that reads as note 0. Note 0 under that track's transpose of -24 is
// 60 semitones below the instrument's base note, so a 792-sample 16 kHz wave
// came out five octaves down and ran 1669 ms instead of 49:
//
//   [vt] note p2 t0 key 0: -60.91 semitones off base 60 (transpose -24,
//        bend -29, range 2) -> rate 0.0145
//   [vt] chan  0 start: 792 samples, -17 dB10, pan 64, prio 64,
//        rate 0.0145 (16000 Hz wave, 1669 ms)
//
// and a pc walking through data rather than code rarely meets an end-of-track
// with an empty stack, so the track does not die either. Pitched down,
// stretched out, and dragging on: one offset applied twice.
/* The mixer frees channels, but the frame counter and the pitch dump live in
   here behind an anonymous namespace. This is the one seam across it, so a
   channel FREE lands in the same frame-stamped file as the note-on that
   opened it. Without the free records a trace shows what started and never
   what finished, which is precisely the shape a voice leak hides in. */
void sd_pd_end(int ch, int prio, const char *why)
{
    SD_PD("end f=%d ch=%d prio=%d why=%s\n", g_frame, ch, prio, why);
}

// A song's identity that survives a reload: the sequence is read from the file
// into a fresh heap block each time it starts, so its address is not it.
static sd_u32 seq_sig(const sd_u8 *seq, sd_u32 off)
{
    sd_u32 h = 2166136261u;
    for (int i = 0; i < 32; i++) h = (h ^ seq[off + i]) * 16777619u;
    return h;
}

static unsigned long long g_leadSkipUntilMs;
static unsigned long long sd_wall_ms(void)
{
    return (unsigned long long)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}
void sd_seq_skip_lead_rest_arm(int ms)
{
    g_leadSkipUntilMs = sd_wall_ms() + (unsigned long long)(ms > 0 ? ms : 1);
}

int sd_seq_start(int p, const sd_u8 *seqBase, sd_u32 startOff,
                 const sd_u8 *sbnk)
{
    if (p < 0 || p >= SD_PLAYERS || !seqBase) return 0;
    /* PORT_SSEQ_TRACE=1: every start with the stream's identity, so a later
       abnormal track death can be matched to what was actually started on
       that player. The 2026-08-05 session died at pc 6664 on a player whose
       stream could not be named after the fact. */
    {
        static int trace = -1;
        if (trace < 0) trace = getenv("PORT_SSEQ_TRACE") != 0;
        if (trace) {
            const sd_u8 *entry = seqBase + startOff;
            long rel = (entry >= g_sdat.base &&
                        entry < g_sdat.base + g_sdat.size)
                       ? (long)(entry - g_sdat.base) : -1;
            fprintf(stderr, "[sseq] start player %d seq %p + 0x%lx "
                    "(sdat+0x%lx)\n", p, (const void *)seqBase,
                    (unsigned long)startOff, rel);
        }
    }
    SD_VT("play %2d start\n", p);
    sd_seq_stop(p);

    Player &pl = g_pl[p];
    memset(&pl, 0, sizeof pl);
    pl.active = 1;
    pl.startOff = startOff;
    pl.sig = seq_sig(seqBase, startOff);
    pl.seq = seqBase;
    pl.sbnk = sbnk;
    pl.tempo = 120;
    pl.tempoCount = 0;
    pl.volume = 127;
    pl.pan = 64;
    // The player's base channel priority, out of the SDAT record. The ARM7
    // starts every one of a player's tracks at this; the port used to default
    // to 64 and lose it, which inverted the ROM's steal order (see
    // sdat_seq_priority).
    pl.cpr = sdat_seq_priority(seqBase, startOff);
    for (int w = 0; w < SD_PLAYERS && g_wantLeft > 0; w++) {
        if (!g_want[w].active || g_want[w].sig != pl.sig || g_want[w].startOff != startOff)
            continue;
        g_seekPending[p] = g_want[w].tickAge;
        g_want[w].active = 0;
        fprintf(stderr, "[sseq] seek player %d to tick %u (at its start)\n", p,
                (unsigned)g_seekPending[p]);
        break;
    }
    {
        /* A mod may have replaced this music track with a recording. The
           sequence still starts and runs (silently), so every status the ROM
           reads is what it would have been; the stream is what is heard. */
        const int mid = sd_music_for_sequence(seqBase, startOff);
        if (mid >= 0 && sd_mix_stream_start(p, mid)) pl.replaced = 1;
    }

    Track &t0 = pl.tr[0];
    t0.active = 1;
    t0.pc = startOff;
    /* CAMVOICE1 (C2): armed by sd_seq_skip_lead_rest_arm; a sequence whose first command is a rest starts at the command after it. */
    if (g_leadSkipUntilMs && sd_wall_ms() < g_leadSkipUntilMs && seqBase[startOff] == 0x80) {
        sd_u32 q = startOff + 1;
        (void)read_varlen(seqBase, q);
        t0.pc = q;
        g_leadSkipUntilMs = 0;
        SD_VT("play %2d start: leading rest skipped (CAMVOICE1)\n", p);
    }
    t0.volume = 127; t0.expression = 127; t0.pan = 64;
    /* TrackInit at 0x037FDA74: mov r1,#0x40 / strb r1,[r4,#0x12]. The
       player's cpr is NOT folded in here; see the sum at the note-on. */
    t0.bendRange = 2; t0.priority = 64; t0.noteWait = 1;
    t0.portaKey = 60;           /* TrackInit, 0x037FDB28 */
    t0.modSpeed = 16; t0.modRange = 1;          /* 0x037FBFBC */
    t0.envAttack = t0.envDecay = 0xff;          /* 0x037FDB0C */
    t0.envSustain = t0.envRelease = 0xff;

    // A multi-track sequence opens with 0xFE <u16 mask>; track 0's own code
    // follows the 0x93 open-track commands, run_track walks them.
    //
    // THE ARM7 BUILDS THE MASK'S TRACKS RIGHT HERE. The START command
    // (0x037FE2C0) peeks the sequence's first byte and, when it is 0xFE, runs
    // TrackInit on one track per mask bit 1..15 (loop at 0x037FE37C..0x037FE3A0)
    // and files them in the player's track table, all before the next command
    // of the batch. TRACK_PARAM and MUTE_TRACKS skip a track that is not in
    // that table (GetPlayerTrack, 0x037FD9CC), so what the game sends in the
    // same batch as the START reaches the tracks only because they already
    // exist. Jolly Roger Bay / Dire Dire Docks send exactly that, a volume 0 and
    // a mute for both layers, to start without the upbeat tracks.
    {
        const sd_u8 *q = seqBase + startOff;
        if (q[0] == 0xfe) {
            unsigned mask = (unsigned)q[1] | ((unsigned)q[2] << 8);
            for (int t = 1; t < SD_TRACKS; t++) {
                if (!(mask & (1u << t))) continue;
                Track &tk = pl.tr[t];
                track_init_open(tk, 0);
                tk.active = 0;
                tk.allocd = 1;
            }
        }
    }
    return 1;
}

void sd_seq_stop(int p)
{
    if (p < 0 || p >= SD_PLAYERS) return;
    if (g_pl[p].active) SD_VT("play %2d stop\n", p);
    if (g_pl[p].replaced) sd_mix_stream_stop(p);
    player_silence(p);
    memset(&g_pl[p], 0, sizeof g_pl[p]);
}

/* COMMAND 0x02, PAUSE_SEQ: the ARM7's handler at 0x037FE09C (arm7.bin case 2
 * of the command switch at 0x037FEBF0), in full:
 *
 *     player->pause = flag;                        strb r1, [r6, #1]
 *     if (flag)
 *         for (t = 0; t < 16; t++)
 *             if ((track = GetPlayerTrack(player, t)) != NULL)
 *                 ReleaseTrackChannels(track, player, 127);   0x037FD948
 *
 * ReleaseTrackChannels gives every channel the track holds release rate 127
 * (the fastest fade), drops it to priority 1, releases it and takes it off the
 * track: track_stop's release + detach, with the rate set first. Nothing is
 * stopped and nothing is rewound, so the tracks keep their place in the
 * stream. SND_SeqMain (0x037FE288) then tests the byte before it steps a
 * player:
 *
 *     if (doPeriodic && !player->pause) PlayerSeqMain(player);
 *     PlayerUpdateChannel(player);
 *
 * so a paused sequence stands still, and flag 0 carries on from the same tick.
 * The game sends it from Sound::PauseMusic / Sound::UnpauseMusic through
 * func_0204f958 -> func_0204f558 -> func_0205ad6c, on the pause menu's way in
 * and out of a course. The player stays active the whole time (its status bit
 * does not clear), which is what the ARM9 expects to read back. */
void sd_seq_pause(int p, int flag)
{
    if (p < 0 || p >= SD_PLAYERS) return;
    Player &pl = g_pl[p];
    pl.paused = flag;
    if (pl.replaced) sd_mix_stream_pause(p, flag);
    if (!flag || !pl.active) return;
    SD_VT("play %2d pause\n", p);
    for (int t = 0; t < SD_TRACKS; t++)
        for (int i = 0; i < SD_CHANNELS; i++)
            if (g_note[i].active && g_note[i].player == p &&
                g_note[i].track == t) {
                sd_mix_set_env(i, -1, -1, -1, 127);
                note_release(i, "sequence paused");
                note_detach(i);
            }
}

void sd_seq_set_volume(int p, int v)
{
    if (p < 0 || p >= SD_PLAYERS || !g_pl[p].active) return;
    g_pl[p].volume = v < 0 ? 0 : (v > 127 ? 127 : v);
}

/* PLAYER_PARAM 4, the byte at player +4: the channel priority every note of
 * this player adds its track's to (0x037FD4E4). sd_seq_start seeds it from
 * the SDAT record, and the game then sends the same record byte here. */
void sd_seq_set_priority(int p, int prio)
{
    if (p < 0 || p >= SD_PLAYERS || !g_pl[p].active) return;
    g_pl[p].cpr = prio & 0xff;
}

/* ONE PLACE THAT KNOWS HOW A SOUNDING VOICE'S PITCH IS MADE UP, the twin of
 * retune_note_vol below and for the same reason.
 *
 * A RELEASED VOICE IS SKIPPED, and that is the ROM's rule rather than a
 * convenience: TrackUpdateChannel walks its track's channel list and its
 * third line is
 *
 *     if (chn->envStatus == 3) continue;      // SND_ENV_RELEASE
 *
 * so a channel already running its release tail keeps whatever pitch it had
 * when the note-off went out. NoteSlot.released is that same bit -- it is set
 * by note_release, which is what sends the note-off -- so the test reads the
 * same state the ROM tests. */
static void retune_note_pitch(int i)
{
    if (!g_note[i].active || g_note[i].released) return;
    const Track &tk = g_pl[g_note[i].player].tr[g_note[i].track];
    int units = track_pitch_units(tk);
    if (units != g_note[i].lastUnits) {
        SD_PD("pit f=%d p=%d t=%d ch=%d bend=%d range=%d ext=%d pu=%d "
              "was=%d rate=%.9f\n", g_frame, g_note[i].player,
              g_note[i].track, i, tk.bend, tk.bendRange, tk.pitch, units,
              g_note[i].lastUnits,
              g_note[i].baseRate * pitch_units_scale(units));
        g_note[i].lastUnits = units;
    }
    /* The channel turns it into a rate, together with its own sweep (see
       sd_mix_set_user_pitch). The same refusal start_note applies happens
       there: a voice already sounding is left at the rate it has rather than
       stopped, as the ROM's own driver clamps the timer reload (SND_CalcTimer
       returns 0xFFFF at the ends) rather than silencing a live note. */
    sd_mix_set_user_pitch(i, units);
}

/* PlayerUpdateChannel: SND_SeqMain runs PlayerSeqMain (all this frame's ticks)
 * and then this, once per 192 Hz frame, for every player that is playing. It
 * is what makes the pitch bend a live control rather than a note property.
 *
 * The port has no per-track channel list -- ownership is recorded against the
 * channel instead (see NoteSlot) -- so the same relation is walked the other
 * way round: sixteen tests per player per frame. */
static void retune_note_vol(int i);

static void player_update_channels(int p)
{
    for (int i = 0; i < SD_CHANNELS; i++) {
        if (!g_note[i].active || g_note[i].player != p) continue;
        retune_note_pitch(i);
        /* The track's modulation fields onto the channel (0x037FD710..
           0x037FD724), on the same channels the pitch reaches. */
        if (!g_note[i].released) {
            const Track &tk = g_pl[p].tr[g_note[i].track];
            sd_mix_set_lfo(i, tk.modTarget, tk.modSpeed, tk.modDepth,
                           tk.modRange, tk.modDelay);
            /* AND THE TRACK'S VOLUME, the same update's first line
               (0x037FD684..0x037FD6C0): SQ[player volume] + SQ[track volume]
               + SQ[track expression] + both faders into ch+0x0C, every
               frame. The port folded the three 0..127 levels into the note
               once, at its note-on, so a 0xC1 / 0xD5 / 0xC2 ramp written
               while a note held -- a fade-in, a fade-out, a swell -- never
               reached the note; it only moved the NEXT note's level. */
            const int b = note_level_db10(g_pl[p], tk, g_note[i].velocity);
            if (b != g_note[i].baseDb10) {
                g_note[i].baseDb10 = b;
                retune_note_vol(i);
            }
        }
    }
}

// ONE PLACE THAT KNOWS HOW A SOUNDING VOICE'S LEVEL IS MADE UP. There are now
// two attenuations riding on a note's own volume, the player's and its
// track's, and three callers that have to retune a voice already sounding.
// Spelling the sum at each of them is how the track term would come to be
// dropped from one of them later.
static void retune_note_vol(int i)
{
    int db10 = g_note[i].baseDb10 + g_pl[g_note[i].player].volDb10
             + g_pl[g_note[i].player].tr[g_note[i].track].volDb10;
    if (db10 < -723) db10 = -723;
    if (db10 > 0) db10 = 0;
    sd_mix_set_vol(i, db10);
}

// PLAYER_PARAM 6, the attenuation func_0204fafc recomputes every frame from
// the voice's distance and its fade ramp. It arrives WHILE the sound plays --
// that is the whole point of it -- so it reaches the notes already sounding,
// the same way the positional pan does below.
void sd_seq_set_volume_db10(int p, int db10)
{
    if (p < 0 || p >= SD_PLAYERS || !g_pl[p].active) return;
    if (db10 > 0) db10 = 0;
    if (db10 < -723) db10 = -723;
    g_pl[p].volDb10 = db10;
    for (int i = 0; i < SD_CHANNELS; i++) {
        if (!g_note[i].active || g_note[i].player != p) continue;
        retune_note_vol(i);
    }
}

/* ---- TRACK_PARAM 0x0a and 0x0c ------------------------------------------
 *
 * WHAT THEY ARE, DERIVED FROM THE DECOMP RATHER THAN FROM A DRIVER HEADER.
 * Command 0x04 has exactly ONE emitter in src/, func_0205a8f0, and it is a
 * one-liner: Snd_SendCommand(4, voice | (size << 24), trackMask, param,
 * value). Five matched TUs call it and each pins one param to one constant:
 *
 *   func_0205ac5c  param 0x19  size 1
 *   func_0205ac84  param 0x1a  size 1
 *   func_0205acac  param 0x09  size 1   pan          (already hosted)
 *   func_0205acfc  param 0x0a  size 2   THIS FILE
 *   func_0205acd4  param 0x0c  size 2   THIS FILE
 *
 * 0x0a IS AN ATTENUATION IN TENTHS OF A DECIBEL. Its only ARM9 feeder,
 * src/func_0204f82c.c, does not pass a number through -- it passes
 * data_02086384[idx]. That array is 0x100 bytes of arm9 rodata
 * (config/arm9/symbols.txt: data_02086384 at 0x02086384, next symbol
 * data_02086484), 128 s16 running -723, -421, -361 ... -1, 0: monotone, ends
 * at 0, bottoms at -723. It is the NITRO volume-to-decibel table, and the
 * proof it is being used AS one is that src/func_0204fafc.c builds
 * PLAYER_PARAM 6 by summing three lookups into THE SAME array and clamping
 * the sum to -0x2d3 (-723). So 0x0a is player param 6's per-track twin: same
 * table, same units, same clamp, applied to one track instead of all of them.
 *
 * 0x0c IS A PITCH OFFSET IN 1/64 OF A SEMITONE. Two matched callers fix the
 * scale between them. src/unnamed/ov007/func_ov007_020bdbcc.c sweeps
 *   -(((0x1000 - t) * 0x300) >> 12),  t in 0..0x1000
 * so full deflection is -0x300 = -768, and 768 units is what this file's own
 * bend arithmetic already calls an octave. src/unnamed/arm9/0204/func_02048af4.c sends
 * (height - 0x28) * 2 from the 3D updater, a few semitones either side of a
 * reference height. Both are signed and both are re-sent every frame, which
 * is why the retune below starts from baseRate.
 *
 * THE TRACK MASK IS A MASK. b is 0xffff from func_02012860, 0xf from the
 * ov007 pair and 3 from func_02048af4; a sound effect is one track but the
 * game does not assume that, and neither does this.
 */
void sd_seq_set_track_volume_db10(int p, unsigned trackMask, int db10)
{
    if (p < 0 || p >= SD_PLAYERS || !g_pl[p].active) return;
    if (db10 > 0) db10 = 0;
    if (db10 < -723) db10 = -723;
    for (int t = 0; t < SD_TRACKS; t++)
        if (trackMask & (1u << t)) g_pl[p].tr[t].volDb10 = db10;
    for (int i = 0; i < SD_CHANNELS; i++) {
        if (!g_note[i].active || g_note[i].player != p) continue;
        if (!(trackMask & (1u << g_note[i].track))) continue;
        retune_note_vol(i);
    }
}

void sd_seq_set_track_pitch(int p, unsigned trackMask, int pitch)
{
    if (p < 0 || p >= SD_PLAYERS || !g_pl[p].active) return;
    /* The ARM7 field is 16 bits and the value arrived as one (size 2 in the
       command), so a caller that sends 0x10000 means 0. Truncating here
       rather than trusting the queue keeps the host in the ROM's domain. */
    pitch = (int)(short)(unsigned short)pitch;
    for (int t = 0; t < SD_TRACKS; t++)
        if (trackMask & (1u << t)) g_pl[p].tr[t].pitch = pitch;
    /* STORING IS THE WHOLE COMMAND. On hardware TRACK_PARAM 0x0c writes
       track->extPitch and stops; the value reaches the sounding channels on
       the next TrackUpdateChannel, which is at most one 192 Hz frame away.
       player_update_channels is that call here, so the arithmetic that used
       to be spelled out again at this call site is gone -- the bend and the
       ext trim are one quantity now (track_pitch_units) and duplicating half
       of it here is how the two would come apart later. */
    for (int i = 0; i < SD_CHANNELS; i++) {
        if (!g_note[i].active || g_note[i].player != p) continue;
        if (!(trackMask & (1u << g_note[i].track))) continue;
        retune_note_pitch(i);
    }
}

/* TRACK_PARAM 0x19 and 0x1a: the track's modulation SPEED and DEPTH (track
 * +0x19 / +0x1a in SM64DS's ARM7, byte-wide: func_0205ac5c and func_0205ac84
 * send them with size 1). Their caller is func_ov007_020bda8c, which ramps the
 * depth 0..0x7f and the speed 5..0x28 on tracks 0xf of one voice every frame.
 * Storing is the whole command; the next track update copies them onto the
 * sounding channels. */
void sd_seq_set_track_mod(int p, unsigned trackMask, int param, int value)
{
    if (p < 0 || p >= SD_PLAYERS || !g_pl[p].active) return;
    for (int t = 0; t < SD_TRACKS; t++) {
        if (!(trackMask & (1u << t))) continue;
        if (param == 0x19) g_pl[p].tr[t].modSpeed = value & 0xff;
        else if (param == 0x1a) g_pl[p].tr[t].modDepth = value & 0xff;
    }
}

void sd_seq_set_pan(int p, int v)
{
    if (p < 0 || p >= SD_PLAYERS || !g_pl[p].active) return;
    g_pl[p].pan = v < 0 ? 0 : (v > 127 ? 127 : v);
    // Positional pan arrives AFTER the note that needs it: Sound::Play calls
    // Player_PlaySoundEffect (which sends START) and only then func_02048d80
    // (which sends the pan). Applying it to the player alone would leave the
    // sound it was computed for playing dead centre, so retune the voices
    // this player already has ringing.
    for (int i = 0; i < SD_CHANNELS; i++) {
        if (!g_note[i].active || g_note[i].player != p) continue;
        int pan = g_note[i].basePan + (g_pl[p].pan - 64);
        sd_mix_set_pan(i, pan < 0 ? 0 : (pan > 127 ? 127 : pan));
    }
}

void sd_seq_frame(void)
{
    // Channels that ended since the last tick come off their tracks first, so
    // a track parked in noteFinishWait sees the release in the same tick the
    // mixer finished it.
    reap_finished_channels();
    g_frame++;
    if (g_wantLeft > 0) g_wantLeft--;

    for (int p = 0; p < SD_PLAYERS; p++) {
        Player &pl = g_pl[p];
        if (!pl.active) continue;

        // A paused player is not stepped (sd_seq_pause), only updated.
        if (!pl.paused) pl.tempoCount += pl.tempo;
        // A pending F9 seek (sd_seq_seek_to): the ticks the song had already
        // played, run now in one go and silently.
        const int seekRun = g_seekPending[p] > 0 && !pl.paused;
        if (seekRun) {
            pl.tempoCount += 240 * g_seekPending[p];
            g_seekPending[p] = 0;
            g_seeking = 1;
        }
        while (!pl.paused && pl.tempoCount >= 240) {
            pl.tempoCount -= 240;
            pl.tickAge++;

            // Note durations are in sequencer TICKS, so they expire here --
            // inside the tempo loop -- not once per 192 Hz frame. A note with
            // ticks < 0 has no scheduled note-off at all (duration 0 or TIE);
            // one already released is running its tail and still belongs to
            // its track, but there is nothing left to count down.
            for (int i = 0; i < SD_CHANNELS; i++) {
                if (!g_note[i].active || g_note[i].player != p) continue;
                if (g_note[i].ticks < 0 || g_note[i].released) continue;
                if (--g_note[i].ticks <= 0)
                    note_release(i, "note duration expired");
            }

            int any = 0;
            for (int t = 0; t < SD_TRACKS; t++) {
                Track &tk = pl.tr[t];
                if (!tk.active) continue;
                if (tk.wait > 0) tk.wait--;
                if (tk.wait == 0) run_track(pl, p, t);
                if (tk.active) any = 1;
            }
            if (!any) {
                // Every track ended, so every track has been through
                // track_stop and let go of its channels: this is
                // PlayerStepTicks returning "not playing" and PlayerStop
                // following it (SND_seq.c lines 588 and 1274).
                //
                // The status bit clears HERE, with the tails still sounding.
                // playerStatus is ply->flags.active (line 102) and nothing in
                // it looks at the channels. There used to be a "is anything
                // still ringing" test guarding this, and it is gone rather
                // than merely unused: every path that clears tk.active runs
                // track_stop first, so no channel can still be owned by this
                // player once its last track is inactive, and the test could
                // never have been true. The tails are not cut by any of it --
                // they are detached, so the next sound to take this slot
                // cannot silence them either.
                SD_VT("play %2d finished: sequence ended, tails detached\n", p);
                pl.active = 0;
                if (pl.replaced) sd_mix_stream_finish(p);   // the recording plays out
                break;
            }
        }

        g_seeking = 0;

        /* SND_SeqMain: PlayerSeqMain, then PlayerUpdateChannel. The order is
           the point -- this frame's commands have already run, so a PITCHBEND
           written this frame reaches the notes this frame, including the ones
           that were already sounding when it was written. */
        player_update_channels(p);
        /* The recording follows the player's volume and the game's fade (the
           ARM9's PLAYER_PARAM 6), the same two terms every sounding note of
           this player carries, once per 192 Hz tick. */
        if (pl.replaced && pl.active)
            sd_mix_stream_set_db10(p, sd_cnv_vol(pl.volume) + pl.volDb10);
    }
}

// ---- SAVE-STATE MUSIC POSITION (lane TRANS093) -------------------------------
//
// lk6_savestate_load silences the sequencer and starts the level's song again,
// from its first note. The sequencer is deterministic (the same bytes stepped
// the same number of ticks land in the same place), so F8 records how far each
// player had stepped and F9 runs the freshly started song that far, silently.
// Notes that were sounding at the save are not brought back; the next note on
// sounds as it did.
void sd_seq_snapshot(SdSeqPos *out)
{
    for (int p = 0; p < SD_SEQ_PLAYERS; p++) {
        const Player &pl = g_pl[p];
        // Under two seconds in, a restart is not audible and a short sound
        // effect that happens to share the sequence would be cut short.
        out[p].active = (pl.active && sd_seq_active(p) && pl.tickAge >= 192) ? 1 : 0;
        out[p].sig = pl.sig;
        out[p].startOff = pl.startOff;
        out[p].tickAge = pl.tickAge;
    }
}

int sd_seq_seek_to(const SdSeqPos *in)
{
    // The game hands out player numbers in the order it asks for sounds, so the
    // song comes back on a different player than it had at the save: it is
    // found by its identity, and waits for its start if it has not begun yet.
    memcpy(g_want, in, sizeof g_want);
    g_wantLeft = 192 * 4;
    int n = 0;
    for (int w = 0; w < SD_SEQ_PLAYERS; w++) {
        if (!g_want[w].active) continue;
        for (int p = 0; p < SD_SEQ_PLAYERS; p++) {
            const Player &pl = g_pl[p];
            if (!pl.active || pl.sig != g_want[w].sig || pl.startOff != g_want[w].startOff ||
                g_want[w].tickAge <= pl.tickAge || g_seekPending[p])
                continue;
            g_seekPending[p] = g_want[w].tickAge - pl.tickAge;
            g_want[w].active = 0;
            fprintf(stderr, "[sseq] seek player %d to tick %u (now %u)\n", p,
                    (unsigned)g_want[w].tickAge, (unsigned)pl.tickAge);
            n++;
            break;
        }
    }
    return n;
}

// SEAMSND1: is any running player still reading this byte range (a retired
// sound image, hal/sdat/sdat.cpp sdat_retired_sweep)? Read only.
int sd_seq_uses_range(const sd_u8 *lo, const sd_u8 *hi)
{
    for (int p = 0; p < SD_PLAYERS; p++) {
        if (!g_pl[p].active) continue;
        if ((g_pl[p].seq >= lo && g_pl[p].seq < hi) || (g_pl[p].sbnk >= lo && g_pl[p].sbnk < hi)) return 1;
    }
    return 0;
}
