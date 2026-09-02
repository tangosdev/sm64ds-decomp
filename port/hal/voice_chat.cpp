/* Proximity voice chat, the game half. Lane VOICE.
 *
 * hal/voice_chat.h carries the datagram, the settings and the test hooks. This
 * file is the machine: capture -> IMA ADPCM -> the seam's aux channel ->
 * a per-peer jitter buffer -> a distance-attenuated add into the finished
 * stereo mix.
 *
 * =========================================================================
 * THE ONE PROPERTY THAT MATTERS: THIS DOES NOT TOUCH THE GAME
 * =========================================================================
 * Everything below reads game state and writes none of it. The two reads are
 * data_0209f394[slot] (the per-slot Player pointers) and data_0209f250 (which
 * slot this console is), both taken as raw words at the offsets
 * hal/comms_sync.cpp and hal/player_fields.h already use, and both used only
 * to compute a floating-point volume that never re-enters the simulation.
 *
 * It is not on the lockstep path. func_0203ea5c.c's exchange is what the game
 * blocks on; voice_tick runs after it, in the same place sync_tick does, and
 * every send goes out through send_aux which the seam's own ordering rule puts
 * behind the input record. A dropped voice datagram is 40 ms of silence and
 * nothing else -- there is no retransmit, no ack, and no state on either side
 * that a loss can corrupt, because every ADPCM block carries its own predictor
 * (see the header).
 *
 * It runs on ONE THREAD. Capture is polled (CALLBACK_NULL, see
 * hal/voice_capture_win.cpp), voice_tick is called from the frame loop, and
 * the mix hook is called from sd_mix_render, which the same frame loop drives
 * through sd_out_push. So there is no lock anywhere in this file and no
 * cross-thread hand-off to get wrong.
 *
 * =========================================================================
 * THE FALLOFF
 * =========================================================================
 * Full VoiceVolume inside VoiceNearRadius, silence at and beyond
 * VoiceFarRadius, and between them
 *
 *     gain = log(far / d) / log(far / near)
 *
 * which is 1 at the near radius, 0 at the far one, and falls fastest where the
 * listener is closest -- the shape distance actually has, and the shape a
 * linear ramp does not. The radii are in the game's own world units, the
 * integer part of the Fix12 positions at Actor+0x5c.
 *
 * =========================================================================
 * MONO, DELIBERATELY
 * =========================================================================
 * Every remote voice is mixed at equal level into both channels. Panning was
 * scoped and dropped rather than guessed: it needs the world handedness of the
 * heading word at Actor+0x8e, this lane derived no measurement for that, and a
 * left/right image that is backwards is worse than no image at all. The seam
 * for it is the pan field on RemoteVoice, which is written once and read once.
 */
#include "hal/voice_chat.h"

#include "hal/comms_seam.h"
#include "hal/comms_loopback.h"
#include "hal/host_settings.h"
#include "hal/sdat/sdat.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

extern "C" {
// The per-slot Player pointers and which slot this console is. Same two
// symbols hal/comms_sync.cpp reads, declared the same way.
extern void *data_0209f394[];
extern unsigned char data_0209f250;
}

namespace port {
namespace {

using voice::kCapFrameSamples;   // 320
using voice::kCapRate;           // 16000

// ---------------------------------------------------------------------------
// THE WIRE
// ---------------------------------------------------------------------------
const unsigned kVoiceTagLE = 0x564e5953u;      // 'S','Y','N','V'
enum : int {
    kVersion       = 1,
    kHeaderBytes   = 12,
    kAdpcmBytes    = kCapFrameSamples / 2,     // 160
    kBlockBytes    = 4 + kAdpcmBytes,          // 164
    kFramesPerPkt  = 2,                        // 40 ms, 25 packets a second
    kMaxPktBytes   = kHeaderBytes + kFramesPerPkt * kBlockBytes,   // 340
};

// The carrier's aux message size, and the relay's payload cap. Both are
// checked here as well as where they are defined, because the datagram above
// is assembled in THIS file and a future frame-count change would be made
// here by somebody who never opens comms_loopback.cpp.
static_assert(kMaxPktBytes <= 384, "a voice datagram no longer fits the "
                                   "carrier's aux message size");
static_assert(kMaxPktBytes <= 700, "a voice datagram no longer fits the "
                                   "relay's 700-byte payload cap");

// ---------------------------------------------------------------------------
// IMA ADPCM, 4 bits a sample. The two tables are the format's own.
// ---------------------------------------------------------------------------
const int kStepTab[89] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41,
    45, 50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190,
    209, 230, 253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658, 724,
    796, 876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272,
    2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132,
    7845, 8630, 9493, 10442, 11487, 12635, 13899, 15289, 16818, 18500, 20350,
    22385, 24623, 27086, 29794, 32767
};
const int kIndexTab[16] = {
    -1, -1, -1, -1, 2, 4, 6, 8, -1, -1, -1, -1, 2, 4, 6, 8
};

struct Codec { int pred; int index; };

inline void codec_clamp(Codec &c)
{
    if (c.pred > 32767) c.pred = 32767;
    if (c.pred < -32768) c.pred = -32768;
    if (c.index < 0) c.index = 0;
    if (c.index > 88) c.index = 88;
}

unsigned char adpcm_encode_one(Codec &c, int sample)
{
    const int step = kStepTab[c.index];
    int diff = sample - c.pred;
    unsigned char code = 0;
    if (diff < 0) { code = 8; diff = -diff; }

    /* The format's own three-bit magnitude search, written out rather than
       looped, because the reference decoder's reconstruction below has to
       mirror it exactly and a clever loop here is where the two drift. */
    int tmp = step;
    int delta = step >> 3;
    if (diff >= tmp) { code |= 4; diff -= tmp; delta += tmp; }
    tmp >>= 1;
    if (diff >= tmp) { code |= 2; diff -= tmp; delta += tmp; }
    tmp >>= 1;
    if (diff >= tmp) { code |= 1; delta += tmp; }

    if (code & 8) c.pred -= delta; else c.pred += delta;
    c.index += kIndexTab[code];
    codec_clamp(c);
    return code;
}

int adpcm_decode_one(Codec &c, unsigned char code)
{
    const int step = kStepTab[c.index];
    int delta = step >> 3;
    if (code & 4) delta += step;
    if (code & 2) delta += step >> 1;
    if (code & 1) delta += step >> 2;
    if (code & 8) c.pred -= delta; else c.pred += delta;
    c.index += kIndexTab[code];
    codec_clamp(c);
    return c.pred;
}

/* One 20 ms frame into one 164-byte block. The block's header is the codec
   state BEFORE the first sample, so the decoder starts exactly where the
   encoder did and a lost frame costs nothing beyond itself. */
void encode_frame(const short *pcm, unsigned char *block)
{
    Codec c;
    c.pred = pcm[0];
    c.index = 0;
    /* The starting predictor is the frame's first sample, which makes the
       first residual zero and costs one sample of accuracy at a frame edge in
       exchange for never carrying state across a datagram. */
    block[0] = (unsigned char)(c.pred & 0xff);
    block[1] = (unsigned char)((c.pred >> 8) & 0xff);
    block[2] = (unsigned char)c.index;
    block[3] = 0;
    unsigned char *out = block + 4;
    for (int i = 0; i < kCapFrameSamples; i += 2) {
        const unsigned char lo = adpcm_encode_one(c, pcm[i]);
        const unsigned char hi = adpcm_encode_one(c, pcm[i + 1]);
        out[i / 2] = (unsigned char)(lo | (hi << 4));
    }
}

void decode_frame(const unsigned char *block, short *pcm)
{
    Codec c;
    c.pred = (short)((unsigned short)block[0] | ((unsigned short)block[1] << 8));
    c.index = block[2];
    codec_clamp(c);
    const unsigned char *in = block + 4;
    for (int i = 0; i < kCapFrameSamples; i += 2) {
        const unsigned char b = in[i / 2];
        pcm[i]     = (short)adpcm_decode_one(c, (unsigned char)(b & 0x0f));
        pcm[i + 1] = (short)adpcm_decode_one(c, (unsigned char)(b >> 4));
    }
}

// ---------------------------------------------------------------------------
// THE JITTER BUFFER, one per remote slot.
//
// A ring of decoded 16 kHz samples with a fractional read cursor. It PRIMES:
// nothing is played until kPrimeFrames whole frames are sitting in it, which
// is what turns a network that delivers 40 ms in bursts into a continuous
// stream. It stops when it runs dry and re-primes rather than stuttering
// through an empty ring, because a buffer that plays whatever is there is a
// buffer that plays clicks.
//
// TWO TO FOUR FRAMES was the range asked for; three is the default, and it is
// the compromise the number has to be: two survives no reordering at all and
// four is 80 ms of added mouth-to-ear delay on top of the 40 ms the packing
// already costs.
// ---------------------------------------------------------------------------
enum : int {
    kPrimeFrames = 3,
    kMaxFrames   = 12,                              // 240 ms hard ceiling
    kRingSamples = kMaxFrames * kCapFrameSamples,
};

struct RemoteVoice {
    short pcm[kRingSamples];
    int   wr;                 // next write index
    int   rd;                 // next read index
    int   avail;              // samples between them
    unsigned frac;            // 16.16 position inside the sample at rd
    int   playing;
    unsigned last_seq;
    int   have_seq;
    unsigned last_ms;         // arrival of the last accepted datagram
    float gain;               // 0..1, distance * VoiceVolume, set by the tick
    float cur_gain;           // what the mixer is actually at, ramped
    float pan;                // 0.5 everywhere; see the banner
    int   dist;               // world units, for the report line
};

RemoteVoice g_rv[kCommsMaxPlayers];

void rv_reset(RemoteVoice &r)
{
    r.wr = r.rd = r.avail = 0;
    r.frac = 0;
    r.playing = 0;
    r.have_seq = 0;
    r.last_seq = 0;
    r.gain = r.cur_gain = 0.0f;
    r.pan = 0.5f;
    r.dist = -1;
}

void rv_push(RemoteVoice &r, const short *frame)
{
    /* A ring that is full is a listener who fell far enough behind that the
       oldest audio is stale. Dropping the OLDEST frame keeps the delay bounded
       -- the alternative, refusing the newest, pins the listener at the
       ceiling for the rest of the call. */
    if (r.avail + kCapFrameSamples > kRingSamples) {
        r.rd = (r.rd + kCapFrameSamples) % kRingSamples;
        r.avail -= kCapFrameSamples;
    }
    for (int i = 0; i < kCapFrameSamples; ++i) {
        r.pcm[r.wr] = frame[i];
        r.wr = (r.wr + 1) % kRingSamples;
    }
    r.avail += kCapFrameSamples;
    if (!r.playing && r.avail >= kPrimeFrames * kCapFrameSamples) r.playing = 1;
}

// ---------------------------------------------------------------------------
// STATE
// ---------------------------------------------------------------------------
int  g_inited;
int  g_on;                       // what VoiceEnabled said last tick
int  g_tone;                     // SM64DS_VOICE_TEST_TONE
int  g_report;                   // SM64DS_VOICE_REPORT
int  g_hooked;                   // the mixer hook is registered
int  g_vol;                      // VoiceVolume, 0..100
int  g_near, g_far;              // the two radii, world units
char g_mic[voice::kCapNameBytes];

short g_pending[kFramesPerPkt][kCapFrameSamples];
int   g_pending_n;
unsigned g_tx_seq;
double g_tone_phase;
unsigned g_tone_ms;              // the tone generator's own 20 ms clock

unsigned long long g_cap_frames, g_tx_pkts, g_rx_pkts, g_rx_bad, g_rx_dup;
unsigned g_report_ms;

int env_on(const char *name)
{
    const char *v = getenv(name);
    return v && *v && strcmp(v, "0") != 0;
}

unsigned now_ms()
{
#if defined(_WIN32)
    return GetTickCount();
#else
    return 0;
#endif
}

/* The recording devices, one line each, at first use. Enumeration opens
   nothing, so this is safe to run with VoiceEnabled off -- which is the point:
   a launcher lane building a device list wants the same names this build
   matches against, and a player who cannot get his microphone selected needs
   to see what the game thinks is on the machine. */
void dump_devices()
{
    char names[16][voice::kCapNameBytes];
    const int n = voice::cap_enumerate(names, 16);
    fprintf(stderr, "[voice] %d recording device%s on this machine\n", n,
            n == 1 ? "" : "s");
    for (int i = 0; i < n && i < 16; ++i)
        fprintf(stderr, "[voice]   device %d: '%s'\n", i, names[i]);
    if (n > 16)
        fprintf(stderr, "[voice]   (%d more, not listed)\n", n - 16);
}

// ---------------------------------------------------------------------------
// THE MIX HOOK. Called from sd_mix_render, AFTER the host master volume, at
// SD_MIX_RATE. See mixer.cpp's tail for why after.
//
// 16000 into 32768 is not a whole ratio, so the cursor is 16.16 fixed point
// stepping by 32000 per output frame (16000 * 65536 / 32768) with linear
// interpolation between the two samples it lands between -- the same
// resampling the DS mixer does in hardware and the same the sdat mixer does
// for every other voice.
// ---------------------------------------------------------------------------
enum : unsigned { kResampleStep = (unsigned)((16000ull << 16) / SD_MIX_RATE) };

void voice_mix_render(sd_s16 *dst, int frames)
{
    if (!g_on) return;
    for (int s = 0; s < kCommsMaxPlayers; ++s) {
        RemoteVoice &r = g_rv[s];
        if (!r.playing) { r.cur_gain = r.gain; continue; }
        if (r.gain <= 0.0f && r.cur_gain <= 0.0f) continue;

        /* The gain is ramped across the block rather than stepped at its
           start. A player crossing the near radius, or the launcher moving
           VoiceVolume, changes this number between two frames; applying it as
           a step puts a click in the output every time it moves. */
        const float g0 = r.cur_gain;
        const float g1 = r.gain;
        const float dg = frames > 0 ? (g1 - g0) / (float)frames : 0.0f;

        sd_s16 *o = dst;
        for (int k = 0; k < frames; ++k, o += 2) {
            if (r.avail < 2) { r.playing = 0; break; }   // dry: re-prime
            const int i0 = r.rd;
            const int i1 = (r.rd + 1) % kRingSamples;
            const float f = (float)(r.frac & 0xffff) / 65536.0f;
            const float sample = (float)r.pcm[i0] +
                                 ((float)r.pcm[i1] - (float)r.pcm[i0]) * f;
            const float g = g0 + dg * (float)k;
            const int v = (int)(sample * g);

            int l = o[0] + v;
            int rr = o[1] + v;
            o[0] = (sd_s16)(l < -32768 ? -32768 : (l > 32767 ? 32767 : l));
            o[1] = (sd_s16)(rr < -32768 ? -32768 : (rr > 32767 ? 32767 : rr));

            r.frac += kResampleStep;
            const int step = (int)(r.frac >> 16);
            if (step) {
                r.frac &= 0xffff;
                r.rd = (r.rd + step) % kRingSamples;
                r.avail -= step;
                if (r.avail < 0) r.avail = 0;
            }
        }
        r.cur_gain = g1;
    }
}

// ---------------------------------------------------------------------------
// THE FALLOFF. Returns 0..1 for a distance in world units.
// ---------------------------------------------------------------------------
float falloff(int dist)
{
    if (dist <= g_near) return 1.0f;
    if (dist >= g_far) return 0.0f;
    const double num = log((double)g_far / (double)dist);
    const double den = log((double)g_far / (double)g_near);
    if (den <= 0.0) return 0.0f;
    double g = num / den;
    if (g < 0.0) g = 0.0;
    if (g > 1.0) g = 1.0;
    return (float)g;
}

/* Distance between two slots' bodies, in world units, or -1 when either body
   is not mounted. The three words at +0x5c are Fix12; the shift to whole units
   happens BEFORE the multiply so the squares cannot overflow a 32-bit int on
   an arena-sized separation, and the result is the number the two radii are
   expressed in. */
int slot_distance(int a, int b)
{
    const char *pa = (const char *)data_0209f394[a];
    const char *pb = (const char *)data_0209f394[b];
    if (!pa || !pb) return -1;
    const int dx = (*(const int *)(pa + 0x5c) >> 12) -
                   (*(const int *)(pb + 0x5c) >> 12);
    const int dy = (*(const int *)(pa + 0x60) >> 12) -
                   (*(const int *)(pb + 0x60) >> 12);
    const int dz = (*(const int *)(pa + 0x64) >> 12) -
                   (*(const int *)(pb + 0x64) >> 12);
    const double d2 = (double)dx * dx + (double)dy * dy + (double)dz * dz;
    return (int)sqrt(d2);
}

// ---------------------------------------------------------------------------
// SETTINGS, every tick. Cheap: host_settings_poll does the file work on its
// own schedule and these five accessors are variable reads.
// ---------------------------------------------------------------------------
void refresh_settings()
{
    const int want = host_setting_voice_enabled() ? 1 : 0;
    g_vol = host_setting_voice_volume();
    g_near = host_setting_voice_near_radius();
    g_far = host_setting_voice_far_radius();
    const char *mic = host_setting_voice_mic_device();

    if (want != g_on) {
        g_on = want;
        fprintf(stderr, "[voice] VoiceEnabled -> %s\n", g_on ? "ON" : "off");
        if (!g_on) {
            /* OFF MEANS THE DEVICE IS GONE, not muted. This is the whole
               promise the header makes about a player's microphone, and it is
               one call, here. Every jitter buffer is dropped with it so a
               toggle back on does not replay 200 ms of what was said before
               the player turned it off. */
            voice::cap_close();
            for (int i = 0; i < kCommsMaxPlayers; ++i) rv_reset(g_rv[i]);
            g_pending_n = 0;
        }
    }
    if (!g_on) return;

    /* The tone generator is a REPLACEMENT for the device, not a source mixed
       beside it: a proof window must not depend on the machine it runs on
       having a microphone, and must not open one. */
    if (g_tone) return;

    if (strcmp(mic, g_mic) != 0 || !voice::cap_is_open()) {
        strncpy(g_mic, mic, sizeof g_mic - 1);
        g_mic[sizeof g_mic - 1] = '\0';
        voice::cap_open(g_mic);
    }
}

/* 440 Hz at full scale minus 6 dB, in 20 ms frames, phase carried across
   frames so the tone is continuous and a receiver's WAV shows one sine rather
   than a buzz at the frame rate. */
void tone_frame(short *out)
{
    const double w = 2.0 * 3.14159265358979323846 * 440.0 / (double)kCapRate;
    for (int i = 0; i < kCapFrameSamples; ++i) {
        out[i] = (short)(16000.0 * sin(g_tone_phase));
        g_tone_phase += w;
        if (g_tone_phase > 2.0 * 3.14159265358979323846)
            g_tone_phase -= 2.0 * 3.14159265358979323846;
    }
}

void send_pending()
{
    const CommsTransport *t = comms_transport();
    if (!t || !t->send_aux) { g_pending_n = 0; return; }

    unsigned char pkt[kMaxPktBytes];
    memcpy(pkt, &kVoiceTagLE, 4);
    pkt[4] = (unsigned char)kVersion;
    pkt[5] = (unsigned char)(t->slot ? t->slot() : 0);
    pkt[6] = (unsigned char)g_pending_n;
    pkt[7] = 0;
    pkt[8]  = (unsigned char)(g_tx_seq & 0xff);
    pkt[9]  = (unsigned char)((g_tx_seq >> 8) & 0xff);
    pkt[10] = (unsigned char)((g_tx_seq >> 16) & 0xff);
    pkt[11] = (unsigned char)((g_tx_seq >> 24) & 0xff);
    for (int i = 0; i < g_pending_n; ++i)
        encode_frame(g_pending[i], pkt + kHeaderBytes + i * kBlockBytes);

    const int len = kHeaderBytes + g_pending_n * kBlockBytes;
    if (t->send_aux(pkt, len) == len) ++g_tx_pkts;
    g_tx_seq += (unsigned)g_pending_n;
    g_pending_n = 0;
}

/* `send` false means the device is drained and the audio thrown away.
   THAT IS NOT THE SAME AS NOT DRAINING IT. A waveIn ring nobody empties fills
   up in 160 ms and then stops; the moment a session forms, the first eight
   reads hand back 160 ms of what the room sounded like BEFORE the match
   started, all at once. Draining while there is nobody to send to costs one
   memcpy a frame and makes the first thing a peer hears be the present. */
void capture_pump(int send)
{
    short frame[kCapFrameSamples];
    /* At 60 Hz against 20 ms frames there are one or two waiting; the bound is
       the ring depth, and it is a bound rather than a while(1) so a device that
       somehow always reports ready cannot hold the frame loop. */
    for (int n = 0; n < 8; ++n) {
        if (g_tone) {
            /* CLOCKED OFF THE WALL, NOT OFF THE FRAME, and this is a real bug
               avoided rather than tidiness. A 60 Hz loop emitting one 20 ms
               frame per tick produces 60 frames a second where the receiver
               consumes 50, so the jitter ring fills, drops its oldest frame,
               and puts a glitch in the proof's own WAV once a second. Pacing
               it at 20 ms of wall clock makes the generated source behave
               exactly like the device it stands in for. */
            const unsigned t = now_ms();
            if (!g_tone_ms) g_tone_ms = t;
            if ((unsigned)(t - g_tone_ms) < 20u) break;
            g_tone_ms += 20;
            tone_frame(frame);
        } else if (!voice::cap_read_frame(frame)) {
            break;
        }
        ++g_cap_frames;
        if (!send) continue;
        memcpy(g_pending[g_pending_n], frame, sizeof frame);
        ++g_pending_n;
        if (g_pending_n >= kFramesPerPkt) send_pending();
    }
}

void recv_pump()
{
    unsigned char pkt[512];
    int from = -1;
    int n;
    while ((n = comms_recv_voice(pkt, (int)sizeof pkt, &from)) > 0) {
        if (from < 0 || from >= kCommsMaxPlayers) { ++g_rx_bad; continue; }
        if (n < kHeaderBytes + kBlockBytes) { ++g_rx_bad; continue; }
        if (pkt[4] != kVersion) { ++g_rx_bad; continue; }
        const int nf = pkt[6];
        if (nf < 1 || nf > kFramesPerPkt) { ++g_rx_bad; continue; }
        if (n < kHeaderBytes + nf * kBlockBytes) { ++g_rx_bad; continue; }

        const unsigned seq = (unsigned)pkt[8] | ((unsigned)pkt[9] << 8) |
                             ((unsigned)pkt[10] << 16) |
                             ((unsigned)pkt[11] << 24);
        RemoteVoice &r = g_rv[from];
        /* UNORDERED CHANNEL, so a datagram that is not newer than the last one
           accepted is a duplicate or a reordering, and either way playing it
           would put audio back in the buffer that has already been heard.
           Signed difference so the counter's wrap is not a cliff. */
        if (r.have_seq && (int)(seq - r.last_seq) <= 0) { ++g_rx_dup; continue; }
        r.have_seq = 1;
        r.last_seq = seq + (unsigned)(nf - 1);
        r.last_ms = now_ms();
        ++g_rx_pkts;

        short frame[kCapFrameSamples];
        for (int i = 0; i < nf; ++i) {
            decode_frame(pkt + kHeaderBytes + i * kBlockBytes, frame);
            rv_push(r, frame);
        }
    }
}

/* Every remote slot's gain for the frame. A slot with no body, no recent
   audio, or a distance past the far radius goes to zero; the mixer ramps to
   it rather than cutting. */
void update_gains()
{
    const int me = (int)data_0209f250;
    const float vol = (float)g_vol / 100.0f;
    const unsigned t = now_ms();
    for (int s = 0; s < kCommsMaxPlayers; ++s) {
        RemoteVoice &r = g_rv[s];
        if (s == me) { r.gain = 0.0f; r.dist = -1; continue; }
        if (!r.playing && r.avail == 0) { r.gain = 0.0f; r.dist = -1; continue; }
        /* Nothing for 400 ms is a peer who stopped talking or dropped out.
           Dropping the buffer rather than letting it drain keeps a rejoin
           from replaying stale audio. */
        if (r.last_ms && (unsigned)(t - r.last_ms) > 400u) {
            rv_reset(r);
            continue;
        }
        const int d = slot_distance(me, s);
        r.dist = d;
        r.gain = d < 0 ? 0.0f : falloff(d) * vol;
    }
}

void report_if_due()
{
    if (!g_report) return;
    const unsigned t = now_ms();
    if (g_report_ms && (unsigned)(t - g_report_ms) < 1000u) return;
    g_report_ms = t;
    voice_report("1s");
}

}  // namespace

// ---------------------------------------------------------------------------
void voice_tick()
{
    if (!g_inited) {
        g_inited = 1;
        g_tone = env_on("SM64DS_VOICE_TEST_TONE");
        g_report = env_on("SM64DS_VOICE_REPORT");
        for (int i = 0; i < kCommsMaxPlayers; ++i) rv_reset(g_rv[i]);
        if (env_on("SM64DS_VOICE_DEVICES")) dump_devices();
        if (g_tone)
            fprintf(stderr, "[voice] SM64DS_VOICE_TEST_TONE=1: a generated "
                            "440 Hz tone replaces the microphone and NO "
                            "recording device is opened\n");
    }

    refresh_settings();
    if (!g_on) return;

    /* The mixer hook is registered on the first tick that finds voice on and
       never taken back down: the render side reads g_on itself, so a player
       toggling the key off silences it without the hook having to be
       unregistered mid-render from another point in the frame. */
    if (!g_hooked) { g_hooked = 1; sd_mix_set_aux_render(voice_mix_render); }

    /* NO SESSION, NOTHING ON THE WIRE. The device stays open -- it is the
       player's choice, not the session's, and closing and reopening it every
       time a match ends would be audible as a click on the next one -- and it
       is still DRAINED, for the reason capture_pump's own note gives. What
       stops is sending: a player sitting in the menus with voice on is not
       broadcasting into a socket with nobody on it. */
    const CommsReadout rr = comms_readout();
    if (!rr.connected || rr.players <= 1) {
        g_pending_n = 0;
        for (int i = 0; i < kCommsMaxPlayers; ++i)
            if (g_rv[i].playing || g_rv[i].avail) rv_reset(g_rv[i]);
        capture_pump(0);
        return;
    }

    capture_pump(1);
    recv_pump();
    update_gains();
    report_if_due();
}

void voice_report(const char *tag)
{
    char line[512];
    int k = snprintf(line, sizeof line,
                     "[voice] %s: on=%d tone=%d dev=%d cap=%llu tx=%llu "
                     "rx=%llu bad=%llu dup=%llu vol=%d near=%d far=%d",
                     tag ? tag : "-", g_on, g_tone, voice::cap_is_open(),
                     g_cap_frames, g_tx_pkts, g_rx_pkts, g_rx_bad, g_rx_dup,
                     g_vol, g_near, g_far);
    for (int s = 0; s < kCommsMaxPlayers && k > 0 && k < (int)sizeof line; ++s) {
        const RemoteVoice &r = g_rv[s];
        if (!r.playing && r.avail == 0 && r.dist < 0) continue;
        k += snprintf(line + k, sizeof line - (size_t)k,
                      " | s%d d=%d g=%.3f buf=%d", s, r.dist, r.gain,
                      r.avail / kCapFrameSamples);
    }
    fprintf(stderr, "%s\n", line);
}

}  // namespace port
