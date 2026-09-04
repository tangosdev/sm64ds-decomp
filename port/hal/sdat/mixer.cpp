// 16 DS-style mixer channels.
//
// The envelope lives in the DS's own domain: a s32 amplitude in units of
// 1/128 of a tenth of a decibel, running from -92544 (= -723 * 128, i.e.
// -72.3 dB, the DS's silence floor) up to 0. Attack is multiplicative in
// that log domain, decay and release are linear in it, and the rate
// conversion Cnv_Fall below is the ARM7's. The one place this departs from
// hardware is the 0..127 -> decibel table (see sd_cnv_vol in sdat.cpp): the
// ROM's table is in an ARM7 binary nobody has decompiled, so a log curve
// stands in for it.
//
// Resampling is linear interpolation. The DS does the same thing in
// hardware, so this is not a shortcut.
//
// This file also owns the 192 Hz sequencer clock: sd_mix_render slices its
// output at 192 Hz boundaries and ticks the sequencer and every envelope
// there. Driving the sequencer off the AUDIO clock rather than the video
// frame is what the ARM7 does, and it keeps tempo correct even when the
// window's frame rate is not 60.
#include "sdat.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---- the voice trace ----------------------------------------------------
//
// SM64DS_VOICE_TRACE=1. Every allocation and every release of a voice, with
// the reason, at all three levels a sound effect travels through:
//
//   arm9   the 16 voice records func_0204fc40 builds, handed out by
//          func_0204f364 and returned by func_0204f2d4, plus the 3D
//          positional slots func_02048720 hands out. This is the level the
//          game itself budgets against, and the only one that can answer
//          "why did Sound::Play refuse". Printed by sd_vtrace_arm9_census in
//          consumer.cpp, which is where those globals are reachable.
//   play   sequencer players starting, stopping and finishing.
//   chan   the 16 mixer channels, and every note that never got one.
//
// The switch is latched once (sd_mix_reset runs before any sound can) and
// every call site is behind SD_VT, so with the variable off this costs one
// predictable branch per event and nothing else.
extern "C" int g_voice_trace;
int g_voice_trace;

void sd_vtrace(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    fputs("[vt] ", stderr);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

namespace {

enum EnvState { ENV_OFF = 0, ENV_ATTACK, ENV_DECAY, ENV_SUSTAIN, ENV_RELEASE };

const sd_s32 AMPL_MIN = -723 * 128;     // -72.3 dB, in 1/128 of 0.1 dB

struct Channel {
    int active;
    const sd_s16 *pcm;
    sd_u32 total, loopStart;
    int loop;
    double pos, step;

    sd_s32 ampl;
    int state;
    int attackCoef, decayRate, sustainLevel, releaseRate;

    int volDb10;        // combined external volume, tenths of a dB (<= 0)
    int pan;            // 0..127
    int priority;
    unsigned seq;       // bumped on every start, so stale handles are inert
};

Channel g_ch[SD_CHANNELS];

// Frames of 32768 Hz output per 192 Hz sequencer tick, as a fraction.
int g_tickAcc;          // counts output frames * 192

int cnv_attack(int a)
{
    static const sd_u8 lut[19] = {
        0, 1, 5, 14, 26, 38, 51, 63, 73, 84, 92, 100, 109, 116, 123, 127,
        132, 137, 143
    };
    if (a >= 0x6d) return lut[0x7f - a];
    return 255 - a;
}

int cnv_fall(int f)
{
    if (f == 0x7f) return 0xffff;       // instant
    if (f == 0x7e) return 0x3c00;
    if (f < 0x32) return f * 2 + 1;
    return 0x1e00 / (0x7e - f);
}

// Lane VOICE: the post-master-volume render hook. See sd_mix_render's tail.
void (*g_aux_render)(sd_s16 *, int);

double db10_to_gain(int db10)
{
    if (db10 <= -723) return 0.0;
    if (db10 > 0) db10 = 0;
    return pow(10.0, db10 / 200.0);
}

}  // namespace

// port/rollback: the output stage muted while a rewound window is re-run.
// Voices still start and envelopes still advance, so a sound that began
// inside the window is heard from where the replay leaves it; only the bytes
// on the way to the device are zeroed. One int, read on the render path.
static volatile int g_host_mute = 0;
extern "C" void sd_host_mute(int on) { g_host_mute = on ? 1 : 0; }

void sd_mix_reset(void)
{
    static int latched;
    if (!latched) {
        latched = 1;
        g_voice_trace = getenv("SM64DS_VOICE_TRACE") != 0;
    }
    memset(g_ch, 0, sizeof g_ch);
    g_tickAcc = 0;
}

/* THE ROM'S OWN CHANNEL ALLOCATOR, transcribed from the ARM7 driver.
 *
 * The routine is at 0x037FC26C in the ARM7 image (arm7.bin offset 0x043D4;
 * the driver is autoloaded to 0x037F8000, so runtime = fileoff - 0x168 +
 * 0x037F8000). Its 16-channel table is at 0x038075C4, stride 0x54, with the
 * priority a u8 at +0x22 and the volume pair a u16 at +0x24.
 *
 * WHAT IT ACTUALLY DOES, and where this port used to differ:
 *
 *   1. IT SCANS IN A FIXED ORDER THAT IS NOT 0..15. The loop indexes a
 *      16-byte table at 0x03805964 holding
 *          4, 5, 6, 7, 2, 0, 3, 1, 8, 9, 10, 11, 14, 12, 15, 13
 *      Nothing in the ARM7 image ever writes that table. It decides which
 *      channel wins a complete priority-and-volume tie, so scanning 0..15
 *      instead picks a different victim.
 *
 *   2. THERE IS NO "IS THIS CHANNEL BUSY" TEST. This port used to hand back
 *      the first inactive channel outright. The ROM has no such branch: a
 *      channel that has STOPPED is given priority 0 by the per-frame update
 *      (0x037FC5A8, 0x037FC6A4 and 0x037FC7AC all store 0 to +0x22), so it
 *      sorts to the front on its own and needs no special case. Matching that
 *      is why sd_mix_kill and the two envelope-end paths below now zero the
 *      priority.
 *
 *   3. STRICTLY LOWER PRIORITY WINS, then QUIETEST WINS. Equal priority is
 *      broken by the compare at 0x037FBD54, which returns -1 when the
 *      incumbent is LOUDER and the caller then takes the challenger. It is a
 *      STRICT less-than, so an exact tie keeps whichever came first in scan
 *      order. This port had no volume tie-break at all: it kept the lowest
 *      channel INDEX, which is what made channel 0 absorb 274 of the opening's
 *      1157 note starts while channel 15 took 37.
 *
 *   4. A CHANNEL IN ITS RELEASE TAIL KEEPS ITS FULL PRIORITY. It gets no
 *      discount; only a channel that has reached the floor drops to 0. So the
 *      release tails this mixer holds are faithful, and the thing that
 *      resolves them is (3): among equals the fading one is the quietest and
 *      is therefore the one taken.
 *
 *   5. ACCEPT IFF requested >= best. The final test is a single
 *      "cmp P, B / blt return NULL" at 0x037FC328, so an EQUAL priority may
 *      steal. This port already agreed here.
 *
 * The one place the port cannot be bit-exact: the ROM compares
 * (vol & 0xFF) << 4 >> shiftTable[vol >> 8], shiftTable = {0,1,2,4}, i.e. a
 * 0..127 volume paired with a hardware divider. This mixer collapses both into
 * a single logarithmic attenuation, so the ORDERING is identical but the
 * quantisation is finer: two channels the ROM would call exactly equal can be
 * ordered here. That can only change which of two equally loud, equal priority
 * channels is taken, never whether a note is dropped.
 */
namespace {

const int kScanOrder[SD_CHANNELS] = {
    4, 5, 6, 7, 2, 0, 3, 1, 8, 9, 10, 11, 14, 12, 15, 13
};

/* The tie-break quantity, in tenths of a decibel of attenuation (always <= 0;
   quieter is smaller). c.ampl is the envelope in 1/128 of a tenth of a dB and
   c.volDb10 is the already-combined external volume. */
int chan_attenuation_db10(const Channel &c)
{
    return c.volDb10 + (int)(c.ampl / 128);
}

}  // namespace

int sd_mix_alloc(int priority)
{
    int best = -1;
    for (int k = 0; k < SD_CHANNELS; k++) {
        const int i = kScanOrder[k];
        if (best < 0) { best = i; continue; }   /* first candidate, outright */
        if (g_ch[i].priority > g_ch[best].priority) continue;
        if (g_ch[i].priority == g_ch[best].priority
            && chan_attenuation_db10(g_ch[i])
                   >= chan_attenuation_db10(g_ch[best]))
            continue;                           /* strict: a tie keeps the first */
        best = i;
    }
    if (best < 0) return -1;                    /* unreachable: 16 candidates */

    if (priority < g_ch[best].priority) {
        SD_VT("chan ALLOC FAILED: quietest lowest-priority channel is %d at "
              "priority %d, above the requested %d\n", best,
              g_ch[best].priority, priority);
        if (g_voice_trace) {
            /* "all 16 sounding" is not the same claim as "all 16 audible": a
               released channel still holds its slot, and its full priority,
               until its envelope reaches the floor. Printing the state next to
               the priority is what tells a genuinely busy scene apart from a
               pile of release tails. */
            static const char *st[] = { "off", "atk", "dec", "sus", "rel" };
            for (int i = 0; i < SD_CHANNELS; i++)
                sd_vtrace("    chan %2d: prio %3d, %s, atten %d dB10%s\n", i,
                          g_ch[i].priority,
                          st[g_ch[i].state >= 0 && g_ch[i].state <= 4
                             ? g_ch[i].state : 0],
                          chan_attenuation_db10(g_ch[i]),
                          g_ch[i].active ? "" : ", INACTIVE");
        }
        return -1;
    }
    if (g_ch[best].active)
        SD_VT("chan %2d STOLEN for priority %d (victim priority %d, atten "
              "%d dB10)\n", best, priority, g_ch[best].priority,
              chan_attenuation_db10(g_ch[best]));
    g_ch[best].active = 0;
    return best;
}

void sd_mix_start(int ch, const SdatWave *w, const SdatNote *n,
                  int volume_db10, int pan, double rate, int priority)
{
    if (ch < 0 || ch >= SD_CHANNELS || !w || !w->pcm || !w->totalSamples) {
        SD_VT("chan %2d start REFUSED: empty wave\n", ch);
        return;
    }
    // The rate is the field an ear notices and a count cannot: it is sample
    // frames consumed per output frame, so 1.0 plays the wave at the pitch
    // it was recorded at and 0.25 is two octaves down AND four times as
    // long. "Pitched down and stretched and dragging on" is this one number
    // coming out small, and it belongs next to the length it multiplies.
    SD_VT("chan %2d start: %u samples%s, %d dB10, pan %d, prio %d, "
          "rate %.4f (%u Hz wave, %.0f ms)\n", ch,
          (unsigned)w->totalSamples, w->loop ? " looping" : "", volume_db10,
          pan, priority, rate, (unsigned)w->sampleRate,
          rate > 0.0 ? (double)w->totalSamples * 1000.0 / (rate * SD_MIX_RATE)
                     : 0.0);
    Channel &c = g_ch[ch];
    unsigned s = c.seq + 1;
    memset(&c, 0, sizeof c);
    c.seq = s;
    c.active = 1;
    c.pcm = w->pcm;
    c.total = w->totalSamples;
    c.loop = w->loop;
    c.loopStart = w->loopStart;
    c.pos = 0.0;
    c.step = rate;
    c.volDb10 = volume_db10;
    c.pan = pan < 0 ? 0 : (pan > 127 ? 127 : pan);
    c.priority = priority;

    c.ampl = AMPL_MIN;
    c.state = ENV_ATTACK;
    c.attackCoef  = cnv_attack(n ? n->attack : 127);
    c.decayRate   = cnv_fall(n ? n->decay : 127);
    c.releaseRate = cnv_fall(n ? n->release : 127);
    c.sustainLevel = sd_cnv_vol(n ? n->sustain : 127) * 128;
}

void sd_mix_set(int ch, int volume_db10, int pan, double rate)
{
    if (ch < 0 || ch >= SD_CHANNELS || !g_ch[ch].active) return;
    g_ch[ch].volDb10 = volume_db10;
    g_ch[ch].pan = pan < 0 ? 0 : (pan > 127 ? 127 : pan);
    if (rate > 0.0) g_ch[ch].step = rate;
}

void sd_mix_set_pan(int ch, int pan)
{
    if (ch < 0 || ch >= SD_CHANNELS || !g_ch[ch].active) return;
    g_ch[ch].pan = pan < 0 ? 0 : (pan > 127 ? 127 : pan);
}

void sd_mix_set_vol(int ch, int volume_db10)
{
    if (ch < 0 || ch >= SD_CHANNELS || !g_ch[ch].active) return;
    g_ch[ch].volDb10 = volume_db10 < -723 ? -723
                     : (volume_db10 > 0 ? 0 : volume_db10);
}

// Retune a voice already sounding. TRACK_PARAM 0x0c arrives every frame for a
// moving sound, so the pitch of a voice has to be changeable without touching
// its level or its pan -- sd_mix_set above writes all three, and the two the
// caller did not mean to change would ride along.
void sd_mix_set_rate(int ch, double rate)
{
    if (ch < 0 || ch >= SD_CHANNELS || !g_ch[ch].active) return;
    if (rate > 0.0) g_ch[ch].step = rate;
}

void sd_mix_release(int ch, const char *why)
{
    if (ch < 0 || ch >= SD_CHANNELS || !g_ch[ch].active) return;
    SD_VT("chan %2d release: %s\n", ch, why);
    g_ch[ch].state = ENV_RELEASE;
}

void sd_mix_kill(int ch, const char *why)
{
    if (ch < 0 || ch >= SD_CHANNELS) return;
    if (g_ch[ch].active) SD_VT("chan %2d kill: %s\n", ch, why);
    g_ch[ch].active = 0;
    g_ch[ch].state = ENV_OFF;
    g_ch[ch].priority = 0;   /* stopped sorts first, as on the ARM7 */
}

int sd_mix_active(int ch)
{
    return (ch >= 0 && ch < SD_CHANNELS) ? g_ch[ch].active : 0;
}

void sd_mix_frame(void)
{
    for (int i = 0; i < SD_CHANNELS; i++) {
        Channel &c = g_ch[i];
        if (!c.active) continue;
        switch (c.state) {
        case ENV_ATTACK:
            // ampl is negative and climbs toward 0 multiplicatively.
            c.ampl = (sd_s32)(((long long)c.attackCoef * c.ampl) / 255);
            if (c.ampl >= 0) { c.ampl = 0; c.state = ENV_DECAY; }
            break;
        case ENV_DECAY:
            c.ampl -= c.decayRate;
            if (c.ampl <= c.sustainLevel) {
                c.ampl = c.sustainLevel;
                c.state = ENV_SUSTAIN;
            }
            break;
        case ENV_SUSTAIN:
            break;
        case ENV_RELEASE:
            c.ampl -= c.releaseRate;
            if (c.ampl <= AMPL_MIN) {
                SD_VT("chan %2d off: envelope release reached silence\n", i);
                c.active = 0;
                c.state = ENV_OFF;
                c.priority = 0;   /* reached the floor: priority 0 */
            }
            break;
        default:
            break;
        }
    }
}

void sd_seq_frame(void);   // forward: the sequencer shares this clock

void sd_mix_render(sd_s16 *dst, int frames)
{
    memset(dst, 0, (size_t)frames * 2 * sizeof(sd_s16));
    int done = 0;
    while (done < frames) {
        // Frames remaining before the next 192 Hz boundary.
        int need = (SD_MIX_RATE - g_tickAcc + 191) / 192;
        if (need <= 0) need = 1;
        int n = frames - done;
        if (n > need) n = need;

        for (int i = 0; i < SD_CHANNELS; i++) {
            Channel &c = g_ch[i];
            if (!c.active || !c.pcm) continue;
            int envDb10 = c.ampl / 128;
            int total = envDb10 + c.volDb10;
            if (total < -723) total = -723;
            double g = db10_to_gain(total);
            if (g <= 0.0) continue;
            double gl = g * (127 - c.pan) / 127.0;
            double gr = g * c.pan / 127.0;

            sd_s16 *o = dst + (done * 2);
            for (int k = 0; k < n; k++, o += 2) {
                sd_u32 idx = (sd_u32)c.pos;
                if (idx >= c.total) {
                    if (c.loop && c.total > c.loopStart) {
                        double span = (double)(c.total - c.loopStart);
                        c.pos = c.loopStart + fmod(c.pos - c.loopStart, span);
                        idx = (sd_u32)c.pos;
                    } else {
                        SD_VT("chan %2d off: sample ran out (%u samples, "
                              "no loop)\n", i, (unsigned)c.total);
                        c.active = 0;
                        c.priority = 0;   /* stopped: priority 0 */
                        break;
                    }
                }
                double frac = c.pos - (double)idx;
                sd_s32 a = c.pcm[idx];
                sd_s32 b = (idx + 1 < c.total) ? c.pcm[idx + 1]
                         : (c.loop ? c.pcm[c.loopStart] : a);
                double s = a + (b - a) * frac;
                sd_s32 l = (sd_s32)(o[0] + s * gl);
                sd_s32 r = (sd_s32)(o[1] + s * gr);
                o[0] = (sd_s16)(l < -32768 ? -32768 : (l > 32767 ? 32767 : l));
                o[1] = (sd_s16)(r < -32768 ? -32768 : (r > 32767 ? 32767 : r));
                c.pos += c.step;
            }
        }

        done += n;
        g_tickAcc += n * 192;
        while (g_tickAcc >= SD_MIX_RATE) {
            g_tickAcc -= SD_MIX_RATE;
            sd_seq_frame();
            sd_mix_frame();
        }
    }

    // Host master volume: one final scalar on the already-mixed stereo buffer.
    // This is the output stage, not the DS mixer -- every voice, envelope and
    // pan above stayed hardware-faithful; this only trims the summed result on
    // the way out (and so the .wav dump and the device hear the same level).
    // pct == 100 is a no-op fast path; pct == 0 zeroes (the old muted case).
    int pct = g_host_mute ? 0 : out_volume_pct();
    if (pct != 100) {
        for (int i = 0; i < frames * 2; i++) {
            sd_s32 s = (sd_s32)((long long)dst[i] * pct / 100);
            dst[i] = (sd_s16)(s < -32768 ? -32768 : (s > 32767 ? 32767 : s));
        }
    }

    // ---- THE AUX RENDER HOOK, lane VOICE --------------------------------
    //
    // A nullable function pointer and not a direct call, for one reason: this
    // file is linked into targets that have no network and no voice chat, and
    // a hard call would make every one of them fail the link for a feature
    // they do not build. Nothing registers it unless hal/voice_chat.cpp is in
    // the target AND the player turned voice on, so the default build runs one
    // null test per render block and nothing else.
    //
    // AFTER THE MASTER VOLUME ON PURPOSE. Everything above this line is the
    // DS's own mixer plus the host output trim, and the .wav dump downstream
    // is the record of what the game sounded like. Voice chat is not the game:
    // it has its own slider (VoiceVolume) and a player who muted the game to
    // hear his friends should still hear his friends. Running it above the
    // trim would have made SM64DS_VOLUME=0 a second mute for a control that
    // already has one.
    if (g_aux_render) g_aux_render(dst, frames);
}

void sd_mix_set_aux_render(void (*fn)(sd_s16 *, int))
{
    g_aux_render = fn;
}
