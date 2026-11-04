// Audio output: winmm waveOut, loaded dynamically.
//
// Dynamic loading follows walk_window's rule for user32/gdi32 -- resolve the
// library only after ntr::io_init has reserved the fixed DS regions, so the
// loader cannot place a DLL at 0x04000000..0x07ffffff and take an address
// the game needs. sd_out_open is reached from the first hosted ARM7 tick,
// which is well after io_init.
//
// A ring of four 1024-frame headers is 125 ms at 32768 Hz. The device is
// asked for 32768 Hz first (the DS's own mixing rate, so no resampling); if
// it refuses, 48000 Hz with linear interpolation at the output stage.
//
// SM64DS_NO_AUDIO=1 skips the device entirely. The mixer and sequencer still
// run -- clocked off the video frame instead of the device -- so a WAV dump
// and every "did that command do anything" check still work.
//
// THE DEVICE OPENS ON A WORKER THREAD (run perf2, lane AUDIOBOOT). On some
// machines winmm takes seconds to answer its first call: on the box this was
// measured on, waveOutGetNumDevs took 13.4 s and waveOutOpen 7 to 17 s, while
// every later call (prepare, write, reset) took under a millisecond. The open
// used to run on the main thread at the first sound push, so the window sat
// on its first picture for ~25 s before the title appeared. Now the first
// push starts the open on a worker, gives it OPEN_GRACE_MS (a normal device
// is live by then and the run is exactly the old one), and returns.
//
// WHILE THE WORKER WAITS, A VIRTUAL DEVICE DRAINS THE RING. It is the same
// NBUF x g_outFrames ring, refilled by the same loop, and each block counts as
// played when the real clock says a device at mixrate() would have finished
// it -- exactly how waveOut hands a header back. So the mixer and the 192 Hz
// sequencer clock advance at the pace a device that opened instantly would
// have driven them, and not at the no-device clock's 546 frames a push, which
// in a course (30 pushes a second) would have run the music at half speed and
// brought it up late. Nothing reaches the speaker in that window: the player
// hears silence until the device exists, and nothing more is lost than that.
//
// THE HAND-OVER renders nothing. The blocks the virtual device still has in
// flight at that instant are prepared and written to the real device in the
// order they were queued, the oldest from the frame the virtual device has
// reached, so the speaker starts where an instant device would be; then the
// ordinary refill runs for the headers the virtual device had already
// finished -- the same work that push would have done with a device all
// along. No block is rendered twice and none is skipped, so the tick count
// and the sequence state carry straight across. (A device that refuses
// 32768 Hz and opens at 48000 cannot play blocks rendered at 32768: those in
// flight at the hand-over are dropped, at most 125 ms nobody heard.)
//
// TEST KNOBS, all off by default:
//   SM64DS_AUDIO_OPEN_DELAY_MS=<n>  the worker sleeps n ms before it starts, to
//                                   force the hand-over mid-level on a machine
//                                   whose device opens fast.
//   SM64DS_AUDIO_OPEN_SYNC=1        the old shape: the first push blocks until
//                                   the device answers (the "instant device"
//                                   side of an A/B, on one binary).
//   SM64DS_AUDIO_BOOT_TRACE=<path>  one line per push: mode, frames rendered,
//                                   the running total, the 192 Hz tick count
//                                   that total implies, the blocks written.
//   SM64DS_AUDIO_DEV_DUMP=<path>    every block exactly as handed to
//                                   waveOutWrite, raw 16-bit stereo, in order.
//   SM64DS_AUDIO_DEV_SILENT=1       each block is zeroed after that record and
//                                   before the write, so a proof run can mix
//                                   at full volume and still play nothing.
#include "sdat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <atomic>
#include <thread>
#endif

namespace {

inline int mixrate(void) { return sd_mix_rate(); }

// OUT_FRAMES_MAX sizes the ring; g_outFrames is the block actually used: 1024
// frames at 32768 Hz ("ds", 31 ms) and 1536 at 48000 Hz ("hq", 32 ms), so the
// ring holds the same ~125 ms either way.
enum { OUT_FRAMES_MAX = 1536, NBUF = 4, MIX_MAX = 2048, OPEN_GRACE_MS = 200 };
int g_outFrames = 1024;

sd_s16 g_mix[MIX_MAX * 2];      // scratch at mixrate(), stereo interleaved

int g_opened;                   // 0 untried, 1 device live, -1 no device,
                                // 2 opening on the worker (virtual device)
int g_devRate = SD_MIX_RATE;       // set from sd_mix_rate() when the device opens
sd_u32 g_cum;                   // frames rendered since boot, every path

// HOST MASTER VOLUME. SM64DS_VOLUME is an integer 0..100 read once at boot; it
// scales the already-mixed stereo output by vol/100 in linear amplitude
// (50 -> half amplitude, 0 -> silent). This is a host output-stage gain only:
// the DS mixer/sequencer, every per-voice envelope and every pan are untouched,
// so timing and the .wav dump's shape are identical to hardware except for one
// final scalar. Default is 50 (half) when the variable is unset. SM64DS_SOUND=1
// is honoured for back-compat as "full volume" when SM64DS_VOLUME is not given.
// SM64DS_NO_AUDIO=1 remains the stronger "no device at all".
int g_volPct = -1;              // -1 unread, else 0..100

#if defined(_WIN32)
typedef MMRESULT (WINAPI *pfnOpen)(HWAVEOUT *, UINT, const WAVEFORMATEX *,
                                   DWORD_PTR, DWORD_PTR, DWORD);
typedef MMRESULT (WINAPI *pfnHdr)(HWAVEOUT, WAVEHDR *, UINT);
typedef MMRESULT (WINAPI *pfnDev)(HWAVEOUT);

HMODULE  g_lib;
pfnOpen  p_Open;
pfnHdr   p_Prepare, p_Unprepare, p_Write;
pfnDev   p_Reset, p_Close;

/* RING HEALTH. The ring is NBUF x g_outFrames frames, refilled once per video
   frame from sd_out_push, and the device drains it at the device rate no
   matter what the video loop is doing. Three counters, so "it squealed" can be
   answered with a number instead of an ear:

     g_pushes     sd_out_push calls that found a live device (= video frames)
     g_refills    headers handed back to the device across the run
     g_starved    pushes that arrived with EVERY header already DONE, which
                  means the device had played the ring dry and was repeating or
                  outputting nothing while it waited. This is the underrun, and
                  it is the shape a glitch/squeal has.

   sd_out_report prints them at exit. A healthy run has g_starved == 0 and
   g_refills close to (run seconds * device rate / g_outFrames). */
int g_pushes, g_refills, g_starved, g_reported;

HWAVEOUT g_dev;
WAVEHDR  g_hdr[NBUF];
sd_s16  *g_buf[NBUF];

// Resampler state, only used when the device would not take 32768 Hz.
double  g_phase;
sd_s16  g_last[2];

double now_ms(void)
{
    static LARGE_INTEGER f;
    LARGE_INTEGER n;
    if (!f.QuadPart) QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&n);
    return n.QuadPart * 1000.0 / f.QuadPart;
}

int open_at(int rate)
{
    WAVEFORMATEX wf;
    memset(&wf, 0, sizeof wf);
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 2;
    wf.nSamplesPerSec = (DWORD)rate;
    wf.wBitsPerSample = 16;
    wf.nBlockAlign = 4;
    wf.nAvgBytesPerSec = (DWORD)rate * 4;
    return p_Open(&g_dev, WAVE_MAPPER, &wf, 0, 0, CALLBACK_NULL)
           == MMSYSERR_NOERROR;
}

/* ---- the worker's half ------------------------------------------------
   Everything the worker writes (g_lib, the p_ pointers, g_dev, the g_w*
   figures) is written before the one release store to g_async and read by
   the main thread only after the acquire load that sees it, so the ring and
   the headers themselves are never touched by two threads. */
std::atomic<int> g_async;       // 0 still opening, 1 open, -1 failed
int    g_wFail;                 // 1 no winmm, 2 entry points, 3 refused
int    g_wRate;
double g_wLoadMs, g_wOpenMs[2];

int open_device(int delay_ms)
{
    if (delay_ms > 0) Sleep((DWORD)delay_ms);
    const double t0 = now_ms();
    g_lib = LoadLibraryA("winmm.dll");
    if (!g_lib) { g_wFail = 1; return 0; }
    p_Open      = (pfnOpen)GetProcAddress(g_lib, "waveOutOpen");
    p_Prepare   = (pfnHdr) GetProcAddress(g_lib, "waveOutPrepareHeader");
    p_Unprepare = (pfnHdr) GetProcAddress(g_lib, "waveOutUnprepareHeader");
    p_Write     = (pfnHdr) GetProcAddress(g_lib, "waveOutWrite");
    p_Reset     = (pfnDev) GetProcAddress(g_lib, "waveOutReset");
    p_Close     = (pfnDev) GetProcAddress(g_lib, "waveOutClose");
    if (!p_Open || !p_Prepare || !p_Unprepare || !p_Write || !p_Reset || !p_Close) {
        g_wFail = 2;
        return 0;
    }
    const double t1 = now_ms();
    g_wLoadMs = t1 - t0;
    g_wRate = mixrate();
    int ok = open_at(mixrate());
    g_wOpenMs[0] = now_ms() - t1;
    if (!ok && mixrate() != 48000) {     // "hq" already asked for 48000
        const double t2 = now_ms();
        g_wRate = 48000;
        ok = open_at(48000);
        g_wOpenMs[1] = now_ms() - t2;
    }
    if (!ok) { g_wFail = 3; return 0; }
    return 1;
}

void open_worker(int delay_ms)
{
    const int ok = open_device(delay_ms);
    g_async.store(ok ? 1 : -1, std::memory_order_release);
}

/* ---- the virtual device ------------------------------------------------
   Per header: when the virtual device finishes playing it (g_vEnd), the order
   it was queued in (g_vSeq), and where its first frame sits in the rendered
   stream (g_off, the WAV dump's frame index). g_vBusy is the instant the
   virtual device's queue runs dry, so a late refill starts playing when it is
   written and not in the past -- the same idle a starved waveOut has. */
double g_vEnd[NBUF], g_vBusy;
unsigned g_vSeq[NBUF], g_seq;
sd_u32 g_off[NBUF];
int    g_trim = -1;             // the header handed over part-played, if any
int    g_trimFrames;            // how much of it the virtual device played

/* ---- the test trace and the device dump -------------------------------- */
FILE  *g_btFile;
FILE  *g_ddFile;
int    g_ddSilent;
int    g_btPush;
double g_bt0;
char   g_btW[160];
int    g_btWn;

void bt_open(void)
{
    const char *p = getenv("SM64DS_AUDIO_BOOT_TRACE");
    if (p && *p) g_btFile = fopen(p, "w");
    const char *d = getenv("SM64DS_AUDIO_DEV_DUMP");
    if (d && *d) g_ddFile = fopen(d, "wb");
    g_ddSilent = getenv("SM64DS_AUDIO_DEV_SILENT") != 0;
    g_bt0 = now_ms();
    if (g_btFile)
        fprintf(g_btFile, "# n = push, t = ms since the first push, rend = "
                "frames rendered this push, cum = frames since boot, tick = "
                "192 Hz frames that total implies, w = blocks written "
                "(header@first frame)\n");
}

void bt_note(int i)
{
    const int skip = (i == g_trim) ? g_trimFrames : 0;
    if (g_btFile && g_btWn < (int)sizeof g_btW - 16)
        g_btWn += sprintf(g_btW + g_btWn, " %d@%u", i, (unsigned)(g_off[i] + skip));
}

void bt_write(int i)
{
    const int skip = (i == g_trim) ? g_trimFrames : 0;
    if (g_ddFile)
        fwrite(g_buf[i] + skip * 2, sizeof(sd_s16) * 2, g_outFrames - skip, g_ddFile);
    if (g_ddSilent)     // recorded above, then the speaker gets zeros
        memset(g_buf[i], 0, g_outFrames * 2 * sizeof(sd_s16));
    bt_note(i);
}

void bt_line(const char *mode, int free_on_entry, sd_u32 cum0)
{
    if (g_btFile) {
        fprintf(g_btFile, "n=%d t=%.1f mode=%s free=%d rend=%u cum=%u tick=%u w=%s\n",
                g_btPush, now_ms() - g_bt0, mode, free_on_entry,
                (unsigned)(g_cum - cum0), (unsigned)g_cum,
                (unsigned)((unsigned long long)g_cum * 192u / mixrate()),
                g_btWn ? g_btW + 1 : "-");
        fflush(g_btFile);
    }
    g_btWn = 0;
    g_btW[0] = 0;
    ++g_btPush;
}
#endif

// ---- wav dump -----------------------------------------------------------
FILE *g_wav;
sd_u32 g_wavFrames;

void wav_hdr(FILE *f, sd_u32 frames)
{
    sd_u32 dataBytes = frames * 4;
    sd_u32 riff = 36 + dataBytes;
    sd_u32 fmtLen = 16, rate = mixrate(), byteRate = mixrate() * 4;
    sd_u16 fmt = 1, ch = 2, bits = 16, align = 4;
    fwrite("RIFF", 1, 4, f);      fwrite(&riff, 4, 1, f);
    fwrite("WAVEfmt ", 1, 8, f);  fwrite(&fmtLen, 4, 1, f);
    fwrite(&fmt, 2, 1, f);        fwrite(&ch, 2, 1, f);
    fwrite(&rate, 4, 1, f);       fwrite(&byteRate, 4, 1, f);
    fwrite(&align, 2, 1, f);      fwrite(&bits, 2, 1, f);
    fwrite("data", 1, 4, f);      fwrite(&dataBytes, 4, 1, f);
}

// Render `frames` at mixrate() into g_mix and feed the dump.
void render_mix(int frames)
{
    if (frames > MIX_MAX) frames = MIX_MAX;
    sd_mix_render(g_mix, frames);
    sd_wav_write(g_mix, frames);
    g_cum += (sd_u32)frames;
}

/* THE RING-HEALTH LINE, once, at exit. Registered from sd_out_open so it only
   exists on runs that actually opened a device, and guarded so a close
   followed by the atexit cannot print it twice. */
void out_report(void)
{
    if (g_reported) return;
    g_reported = 1;
    fprintf(stderr, "[audio] ring: %d pushes, %d refills, %d starved "
                    "(%d x %d frames at %d Hz = %.0f ms)\n",
            g_pushes, g_refills, g_starved, NBUF, g_outFrames, g_devRate,
            1000.0 * NBUF * g_outFrames / (g_devRate ? g_devRate : 1));
    fflush(stderr);
}

}  // namespace

// Host output-stage master volume, read once from SM64DS_VOLUME. See the
// header on g_volPct above. External linkage: sd_mix_render applies it.
int out_volume_pct(void)
{
    if (g_volPct < 0) {
        const char *v = getenv("SM64DS_VOLUME");
        if (v && *v) {
            long n = strtol(v, 0, 10);
            g_volPct = (int)(n < 0 ? 0 : (n > 100 ? 100 : n));
        } else if (getenv("SM64DS_SOUND")) {
            g_volPct = 100;     // legacy debug switch: full volume
        } else {
            g_volPct = 50;      // default: half volume
        }
    }
    return g_volPct;
}

// The live half of the same knob: the settings.json watcher pushes the file's
// Volume here whenever it moves, and the next mixed buffer wears it. Same
// clamp, same meaning, still only the host output stage.
extern "C" void out_set_volume_pct(int pct)
{
    const int v = pct < 0 ? 0 : (pct > 100 ? 100 : pct);
    if (g_volPct == v) return;
    g_volPct = v;
    fprintf(stderr, "[audio] master volume now %d%%%s\n", v,
            v == 0 ? " (silent)" : "");
}

// run after093 lane MPSOUND1: frames rendered since boot, the WAV dump's sample
// index (hal/coop_sound.cpp names it in its [msnd] lines). Read only.
extern "C" unsigned sd_out_frames_rendered(void) { return (unsigned)g_cum; }

// ---- wav ----------------------------------------------------------------

void sd_wav_open(const char *path)
{
    if (g_wav) return;
    g_wav = fopen(path, "wb");
    if (!g_wav) { fprintf(stderr, "[sdat] cannot write %s\n", path); return; }
    g_wavFrames = 0;
    wav_hdr(g_wav, 0);          // patched on close
    // walk_window exits straight out of its frame loop, so without this the
    // RIFF sizes stay at zero and the file reads as empty even though every
    // sample is in it.
    atexit(sd_wav_close);
    fprintf(stderr, "[sdat] wav dump -> %s\n", path);
}

void sd_wav_write(const sd_s16 *pcm, int frames)
{
    if (!g_wav || frames <= 0) return;
    fwrite(pcm, sizeof(sd_s16) * 2, (size_t)frames, g_wav);
    g_wavFrames += (sd_u32)frames;
}

void sd_wav_close(void)
{
    if (!g_wav) return;
    fseek(g_wav, 0, SEEK_SET);
    wav_hdr(g_wav, g_wavFrames);
    fclose(g_wav);
    g_wav = 0;
    fprintf(stderr, "[sdat] wav dump closed: %u frames (%.2f s)\n",
            g_wavFrames, (double)g_wavFrames / mixrate());
}

// ---- device -------------------------------------------------------------

#if defined(_WIN32)
namespace {

double g_tFirst;                // the first sound push, for the ready line

/* The ring is allocated when the first push starts the open, before the
   device exists, so the virtual device can fill it. The headers are NOT
   prepared here: waveOutPrepareHeader needs the device, and it runs on the
   main thread at the hand-over (go_live). */
void ring_alloc(void)
{
    for (int i = 0; i < NBUF; i++) {
        g_buf[i] = (sd_s16 *)calloc(OUT_FRAMES_MAX * 2, sizeof(sd_s16));
        memset(&g_hdr[i], 0, sizeof g_hdr[i]);
        g_hdr[i].lpData = (LPSTR)g_buf[i];
        g_hdr[i].dwBufferLength = g_outFrames * 2 * sizeof(sd_s16);
        g_hdr[i].dwFlags = WHDR_DONE;       // free to fill
    }
}

/* The main thread's half of the open, once the worker has answered (or, under
   SM64DS_AUDIO_OPEN_SYNC, once open_device returned on this thread). With
   nothing in flight -- the SYNC shape, or a device that answers before the
   second push -- this is exactly what the old open did after waveOutOpen. */
int go_live(int ok)
{
    if (!ok) {
        if (g_wFail == 1)
            fprintf(stderr, "[sdat] winmm.dll not available -- silent\n");
        else if (g_wFail == 2)
            fprintf(stderr, "[sdat] winmm waveOut entry points missing -- silent\n");
        else
            fprintf(stderr, "[sdat] waveOutOpen failed at 32768 and 48000 Hz "
                            "-- silent\n");
        g_opened = -1;
        return 0;
    }
    g_devRate = g_wRate;

    // The blocks the virtual device has not finished BY NOW (not by the last
    // push: one it finished since then is played, and handing it over would
    // play it twice in the timeline), oldest first.
    const double now = now_ms();
    for (int i = 0; i < NBUF; i++)
        if (!(g_hdr[i].dwFlags & WHDR_DONE) && g_vEnd[i] <= now)
            g_hdr[i].dwFlags |= WHDR_DONE;
    int order[NBUF], n = 0;
    for (int i = 0; i < NBUF; i++)
        if (!(g_hdr[i].dwFlags & WHDR_DONE)) order[n++] = i;
    for (int a = 1; a < n; a++)
        for (int b = a; b > 0 && g_vSeq[order[b]] < g_vSeq[order[b - 1]]; b--) {
            const int t = order[b];
            order[b] = order[b - 1];
            order[b - 1] = t;
        }
    // The oldest is part-played: the real device starts where the virtual
    // one is, so its header covers only the frames still to play (and is
    // re-prepared whole when it comes back, in the refill below).
    g_trim = -1;
    g_trimFrames = 0;
    if (n > 0 && g_wRate == mixrate()) {
        const int i = order[0];
        const double start = g_vEnd[i] - g_outFrames * 1000.0 / mixrate();
        int played = now > start ? (int)((now - start) * mixrate() / 1000.0) : 0;
        if (played > g_outFrames - 1) played = g_outFrames - 1;
        if (played > 0) { g_trim = i; g_trimFrames = played; }
    }
    for (int i = 0; i < NBUF; i++) {
        const DWORD done = g_hdr[i].dwFlags & WHDR_DONE;
        g_hdr[i].dwFlags = 0;               // prepare wants it zero
        if (i == g_trim) {
            g_hdr[i].lpData = (LPSTR)(g_buf[i] + g_trimFrames * 2);
            g_hdr[i].dwBufferLength =
                (DWORD)((g_outFrames - g_trimFrames) * 2 * sizeof(sd_s16));
        }
        p_Prepare(g_dev, &g_hdr[i], sizeof(WAVEHDR));
        g_hdr[i].dwFlags |= done;
    }
    int dropped = 0;
    if (g_devRate != mixrate()) {
        // Rendered at 32768 Hz for the virtual device; this device plays
        // 48000 through the resampler, so these cannot be handed over.
        for (int k = 0; k < n; k++) g_hdr[order[k]].dwFlags |= WHDR_DONE;
        dropped = n;
        n = 0;
    }
    for (int k = 0; k < n; k++) {
        bt_write(order[k]);
        p_Write(g_dev, &g_hdr[order[k]], sizeof(WAVEHDR));
    }
    g_opened = 1;
    fprintf(stderr, "[sdat] waveOut open at %d Hz, %d x %d frames (%.0f ms)\n",
            g_devRate, NBUF, g_outFrames,
            1000.0 * NBUF * g_outFrames / g_devRate);
    fprintf(stderr, "[sdat] device ready %.0f ms after the first sound push "
                    "(winmm load %.0f ms, open %.0f ms%s); %d block(s) in "
                    "flight handed over (the first from its frame %d), %d "
                    "dropped\n",
            now - g_tFirst, g_wLoadMs, g_wOpenMs[0] + g_wOpenMs[1],
            g_wOpenMs[1] > 0 ? " incl. the 48000 Hz retry" : "", n,
            g_trimFrames, dropped);
    fprintf(stderr, "[audio] master volume %d%%%s\n", out_volume_pct(),
            out_volume_pct() == 0 ? " (silent)" : "");
    atexit(out_report);
    return 1;
}

/* One push while the worker is still opening: the virtual device hands back
   every header whose block it has finished by now, and the refill below is
   the device path's own, less the resampler (the virtual device plays
   mixrate()) and less the write. */
void virt_push(sd_u32 cum0)
{
    const double now = now_ms();
    int free_on_entry = 0;
    for (int i = 0; i < NBUF; i++) {
        if (!(g_hdr[i].dwFlags & WHDR_DONE) && g_vEnd[i] <= now)
            g_hdr[i].dwFlags |= WHDR_DONE;
        if (g_hdr[i].dwFlags & WHDR_DONE) free_on_entry++;
    }
    for (int i = 0; i < NBUF; i++) {
        if (!(g_hdr[i].dwFlags & WHDR_DONE)) continue;
        g_off[i] = g_cum;
        render_mix(g_outFrames);
        memcpy(g_buf[i], g_mix, g_outFrames * 2 * sizeof(sd_s16));
        g_hdr[i].dwFlags &= ~WHDR_DONE;
        const double start = g_vBusy > now ? g_vBusy : now;
        g_vEnd[i] = g_vBusy = start + g_outFrames * 1000.0 / mixrate();
        g_vSeq[i] = ++g_seq;
        bt_note(i);
    }
    bt_line("virt", free_on_entry, cum0);
}

}  // namespace
#endif

int sd_out_open(void)
{
    if (g_opened) return g_opened == 1;
    g_opened = -1;
    g_outFrames = sd_audio_hq() ? 1536 : 1024;

    if (getenv("SM64DS_NO_AUDIO")) {
        fprintf(stderr, "[sdat] SM64DS_NO_AUDIO=1: no output device, "
                        "mixer runs silently\n");
        return 0;
    }
#if defined(_WIN32)
    const char *dl = getenv("SM64DS_AUDIO_OPEN_DELAY_MS");
    int delay = dl ? atoi(dl) : 0;
    if (delay < 0) delay = 0;
    ring_alloc();
    g_tFirst = now_ms();
    int async = getenv("SM64DS_AUDIO_OPEN_SYNC") == 0;
    if (async) {
        try {
            std::thread(open_worker, delay).detach();
        } catch (...) {
            async = 0;                      // no thread: open here, as before
        }
    }
    if (!async) return go_live(open_device(delay));
    /* THE GRACE. A device that answers in the tens of milliseconds a normal
       machine takes is waited for, here, for up to OPEN_GRACE_MS: then it is
       live before the first block is rendered and nothing at all is lost,
       exactly as when the open ran on this thread. Only a slow device costs
       the boot this much and then goes to the background. */
    while (g_async.load(std::memory_order_acquire) == 0 &&
           now_ms() - g_tFirst < OPEN_GRACE_MS)
        Sleep(1);
    const int st = g_async.load(std::memory_order_acquire);
    if (st != 0) return go_live(st > 0);
    g_opened = 2;
    fprintf(stderr, "[sdat] audio device opening in the background "
                    "(delay %d ms); the ring runs on the real clock until it "
                    "answers\n", delay);
    return 0;
#else
    fprintf(stderr, "[sdat] no audio backend on this platform -- silent\n");
    return 0;
#endif
}

void sd_out_close(void)
{
#if defined(_WIN32)
    if (g_opened == 2) {
        // Still opening: the worker owns g_lib and g_dev until it answers,
        // and a device it opens after this is left to process teardown.
    } else {
        if (g_opened > 0) {
            p_Reset(g_dev);
            for (int i = 0; i < NBUF; i++) {
                p_Unprepare(g_dev, &g_hdr[i], sizeof(WAVEHDR));
                free(g_buf[i]);
                g_buf[i] = 0;
            }
            p_Close(g_dev);
            g_dev = 0;
        }
        if (g_lib) { FreeLibrary(g_lib); g_lib = 0; }
    }
#endif
    g_opened = -1;
    sd_wav_close();
}

void sd_out_push(void)
{
#if defined(_WIN32)
    const sd_u32 cum0 = g_cum;
    const char *mode = "dev";
    if (!g_opened) bt_open();
#endif
    if (!g_opened) sd_out_open();

#if defined(_WIN32)
    if (g_opened == 2) {
        const int st = g_async.load(std::memory_order_acquire);
        if (st == 0) { virt_push(cum0); return; }
        go_live(st > 0);
        mode = "hand";
    }
    if (g_opened > 0) {
        // SM64DS_SND_SLOW_MS: how the push's time splits between the mix
        // render and the device write (see consumer.cpp's [snd-slow] line)
        static double slow_ms = -1;
        if (slow_ms < 0) { const char *e = getenv("SM64DS_SND_SLOW_MS"); slow_ms = e ? atof(e) : 0; }
        double t_mix = 0, t_write = 0;
        int refilled = 0;
        int free_on_entry = 0;
        for (int i = 0; i < NBUF; i++)
            if (g_hdr[i].dwFlags & WHDR_DONE) free_on_entry++;
        g_pushes++;
        /* Every header free means the device finished everything queued before
           this call got here. The first push of the run is the ring being
           filled for the first time and is not a starve. */
        if (free_on_entry == NBUF && g_pushes > 1) g_starved++;
        for (int i = 0; i < NBUF; i++) {
            if (!(g_hdr[i].dwFlags & WHDR_DONE)) continue;
            if (i == g_trim) {
                // back from the hand-over's part block: whole again
                p_Unprepare(g_dev, &g_hdr[i], sizeof(WAVEHDR));
                g_hdr[i].lpData = (LPSTR)g_buf[i];
                g_hdr[i].dwBufferLength = g_outFrames * 2 * sizeof(sd_s16);
                g_hdr[i].dwFlags = 0;
                p_Prepare(g_dev, &g_hdr[i], sizeof(WAVEHDR));
                g_trim = -1;
                g_trimFrames = 0;
            }
            const clock_t c0 = slow_ms > 0 ? clock() : 0;
            g_off[i] = g_cum;
            if (g_devRate == mixrate()) {
                render_mix(g_outFrames);
                memcpy(g_buf[i], g_mix, g_outFrames * 2 * sizeof(sd_s16));
            } else {
                // Linear interpolation up to the device rate. g_phase and
                // g_last carry across blocks so the seam is continuous.
                double ratio = (double)mixrate() / g_devRate;
                int need = (int)(g_outFrames * ratio) + 2;
                render_mix(need);
                sd_s16 *o = g_buf[i];
                for (int k = 0; k < g_outFrames; k++, o += 2) {
                    int idx = (int)g_phase;
                    double f = g_phase - idx;
                    if (idx >= need - 1) idx = need - 2, f = 1.0;
                    for (int c = 0; c < 2; c++) {
                        double a = g_mix[idx * 2 + c], b = g_mix[(idx + 1) * 2 + c];
                        o[c] = (sd_s16)(a + (b - a) * f);
                    }
                    g_phase += ratio;
                }
                g_phase -= need;
                if (g_phase < 0) g_phase = 0;
                g_last[0] = g_mix[(need - 1) * 2];
                g_last[1] = g_mix[(need - 1) * 2 + 1];
            }
            // Master volume was already applied in sd_mix_render (the host
            // output stage), so g_buf holds the level that goes to the speaker.
            g_hdr[i].dwFlags &= ~WHDR_DONE;
            g_hdr[i].dwBufferLength = g_outFrames * 2 * sizeof(sd_s16);
            g_refills++;
            const clock_t c1 = slow_ms > 0 ? clock() : 0;
            bt_write(i);
            p_Write(g_dev, &g_hdr[i], sizeof(WAVEHDR));
            if (slow_ms > 0) {
                const double k = 1000.0 / CLOCKS_PER_SEC;
                t_mix += (c1 - c0) * k;
                t_write += (clock() - c1) * k;
                ++refilled;
            }
        }
        if (slow_ms > 0 && t_mix + t_write >= slow_ms)
            fprintf(stderr, "[snd-slow]   out_push: %d block(s) refilled (%d free on entry), "
                    "mix %.1f ms, waveOutWrite %.1f ms\n", refilled, free_on_entry, t_mix, t_write);
        bt_line(mode, free_on_entry, cum0);
        return;
    }
#endif
    // No device: clock the mixer off the video frame so state still advances
    // (envelopes decay, sequences end) and the dump still fills. A push is 546
    // frames of the DS's 32768 Hz clock whatever the mix rate, so the 192 Hz
    // tick count at every push is the same in "ds" and "hq" (floor(n * 546 *
    // rate / 32768) running total: exact, no drift, ticks agree to the push).
    {
        static unsigned long long pushes, rendered;
        ++pushes;
        const unsigned long long target =
            pushes * 546ull * (unsigned)mixrate() / (unsigned)SD_MIX_RATE;
        render_mix((int)(target - rendered));
        rendered = target;
    }
#if defined(_WIN32)
    bt_line("nodev", -1, cum0);
#endif
}

/* SEAMSND1: frames rendered since boot (read only), so a log line can say where in the output a sound re-seat happened. */
extern "C" unsigned sd_out_frames(void) { return (unsigned)g_cum; }
