/* Microphone capture: winmm waveIn, loaded dynamically. Lane VOICE.
 *
 * MIRRORS hal/sdat/out_win.cpp EXACTLY, and the reason is that file's, not a
 * preference: the library is resolved with LoadLibraryA at first use rather
 * than linked as a static import, because the loader resolves static imports
 * BEFORE the TLS callback that claims 0x02000000..0x07ffffff for the hosted DS
 * regions, and a DLL the loader placed in that range takes an address the game
 * needs. out_win.cpp banks that trap for waveOut; this is the same trap with
 * the same shape, so it gets the same answer. First use here is the first
 * voice_tick with VoiceEnabled on, which is far past io_init.
 *
 * NO THREAD AND NO CALLBACK. The device is opened CALLBACK_NULL and the ring
 * is polled from the frame loop, so every byte of captured audio is handled on
 * the same thread that runs the game and there is nothing to lock. A 60 Hz
 * poll against 20 ms frames means at most two frames are waiting on any tick,
 * and the ring is eight deep (160 ms), so a frame the video loop was late for
 * is still there when it arrives.
 *
 * NOTHING IS OPENED UNLESS THE PLAYER ASKED. cap_open is only ever called from
 * hal/voice_chat.cpp's tick, and only when VoiceEnabled reads true. A default
 * build never reaches winmm's recording half at all -- not to enumerate, not
 * to probe. SM64DS_VOICE_DEVICES=1 is the one exception and it enumerates
 * without opening anything. The auto-pick scan below is inside that rule: it
 * runs on the first cap_open, which is already gated on VoiceEnabled.
 *
 * AUTO-PICK IS THE DEFAULT, AND IT EXISTS BECAUSE "IT OPENED" IS NOT "IT
 * WORKS". waveInOpen returns MMSYSERR_NOERROR for a device that is muted in
 * Windows, blocked by the microphone privacy gate, or has its input level
 * slider at zero. Measured on the owner's machine, per-device, at 16 kHz mono:
 *
 *     device 0  'Microphone (5- Razer BlackShark)'  opens OK, PEAK    1/32767
 *     device 1  'Microphone (Razer Seiren Mini)'    opens OK, PEAK  470/32767
 *
 * Device 0 was the Windows DEFAULT recording device, so it is what this file
 * got, and voice was silent with nothing anywhere reporting an error. The scan
 * below is the one check that separates those two rows: open each device, take
 * 150 ms of samples, and pick the first one whose peak clears a silence floor.
 * It runs ONCE per process (cached in g_auto_done) and it is not on any
 * deterministic path -- see hal/voice_chat.h's cap_open note.
 */
#include "hal/voice_chat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <mmsystem.h>
#endif

namespace port {
namespace voice {
namespace {

#if defined(_WIN32)

enum { NBUF = 8 };   // 8 x 20 ms = 160 ms of headroom

typedef MMRESULT (WINAPI *pfnInOpen)(HWAVEIN *, UINT, const WAVEFORMATEX *,
                                     DWORD_PTR, DWORD_PTR, DWORD);
typedef MMRESULT (WINAPI *pfnInHdr)(HWAVEIN, WAVEHDR *, UINT);
typedef MMRESULT (WINAPI *pfnInDev)(HWAVEIN);
typedef UINT     (WINAPI *pfnInNum)(void);
typedef MMRESULT (WINAPI *pfnInCaps)(UINT_PTR, LPWAVEINCAPSA, UINT);

HMODULE   g_lib;
pfnInOpen p_Open;
pfnInHdr  p_Prepare, p_Unprepare, p_Add;
pfnInDev  p_Start, p_Stop, p_Reset, p_Close;
pfnInNum  p_NumDevs;
pfnInCaps p_Caps;

HWAVEIN g_dev;
WAVEHDR g_hdr[NBUF];
short  *g_buf[NBUF];
int     g_next;                    // which header is due to come back next
int     g_open;
char    g_open_name[kCapNameBytes];
int     g_open_index;              // the VoiceMicIndex the open was made with

/* ---- THE AUTO-PICK SCAN --------------------------------------------------
 *
 * kSilencePeak: the peak, out of 32767, at or under which a device that opened
 * counts as SILENT. The two numbers this machine produced are 1 (the muted
 * default) and 470 (the live mic), and the gap between them is where any
 * threshold in the 20..50 band lands. 32 is the pick: five bits of headroom
 * over a device reading dead, an order of magnitude under the live one, and
 * comfortably below the noise floor a real microphone shows in a quiet room --
 * which is the number that matters, because auto-pick has to work for a player
 * who is not talking while the game boots. It is deliberately NOT the probe's
 * 8: this decides which device a player gets rather than printing a diagnosis,
 * so it wants room over a device whose dead reading dithers a few counts.
 *
 * kScanWindowMs: how long one device is recorded from. The loop returns the
 * moment the peak clears the floor, so a live device usually costs far less
 * than this and only a silent one pays the whole window.
 *
 * kScanBudgetMs: the hard ceiling on the WHOLE scan, so a machine with eight
 * recording devices does not spend a second and a half in cap_open. Devices
 * past the budget are named as unmeasured rather than skipped in silence. On
 * the owner's two-device machine the real cost is one full window on the silent
 * default plus a short early-out on the live mic, about 200 ms, once. */
enum : int { kSilencePeak = 32, kScanWindowMs = 150, kScanBudgetMs = 600 };

int  g_auto_done;                  // the scan has run; g_auto_id is the answer
UINT g_auto_id;                    // WAVE_MAPPER when nothing measured live

/* ---- THE FAILURE LATCH ---------------------------------------------------
 *
 * A box with no recording device -- or one whose microphone another program
 * already holds exclusively -- used to make this file try again on every
 * frame, because the caller's only test was "is a device open", and on such a
 * box it never is. Sixty enumerate-and-open attempts a second, and sixty
 * copies of the failure line in the log with them. That is the same shape
 * load_lib() above already guards against with g_lib, and it gets the same
 * answer here.
 *
 * LATCHED PER NAME, not globally, so plugging in the headset the player named
 * and pointing VoiceMicDevice at it is not blocked by an earlier failure on a
 * different name. cap_rearm() clears it outright, and voice_chat.cpp calls
 * that whenever the player changes the name or turns voice off and on again:
 * somebody who just changed a setting is owed an immediate retry rather than a
 * wait on a timer.
 *
 * THE BACKOFF IS THE CALLER'S (voice_chat.cpp, five seconds), because it owns
 * the clock and the settings. This side only has to stop being loud and stop
 * doing work, and the failure line is printed once per latch. */
int     g_fail;
char    g_fail_name[kCapNameBytes];
int     g_fail_index;              // the VoiceMicIndex that produced the latch

/* Resolve winmm's recording entry points once. 0 means "this machine cannot
   record through this path", which every caller reads as "no voice", never as
   an error worth stopping the game for. */
int load_lib()
{
    if (p_Open) return 1;
    if (g_lib) return 0;                     // tried and failed, do not retry
    g_lib = LoadLibraryA("winmm.dll");
    if (!g_lib) {
        fprintf(stderr, "[voice] winmm.dll not available -- no capture\n");
        return 0;
    }
    p_Open      = (pfnInOpen)GetProcAddress(g_lib, "waveInOpen");
    p_Prepare   = (pfnInHdr) GetProcAddress(g_lib, "waveInPrepareHeader");
    p_Unprepare = (pfnInHdr) GetProcAddress(g_lib, "waveInUnprepareHeader");
    p_Add       = (pfnInHdr) GetProcAddress(g_lib, "waveInAddBuffer");
    p_Start     = (pfnInDev) GetProcAddress(g_lib, "waveInStart");
    p_Stop      = (pfnInDev) GetProcAddress(g_lib, "waveInStop");
    p_Reset     = (pfnInDev) GetProcAddress(g_lib, "waveInReset");
    p_Close     = (pfnInDev) GetProcAddress(g_lib, "waveInClose");
    p_NumDevs   = (pfnInNum) GetProcAddress(g_lib, "waveInGetNumDevs");
    p_Caps      = (pfnInCaps)GetProcAddress(g_lib, "waveInGetDevCapsA");
    if (!p_Open || !p_Prepare || !p_Unprepare || !p_Add || !p_Start ||
        !p_Stop || !p_Reset || !p_Close || !p_NumDevs || !p_Caps) {
        fprintf(stderr, "[voice] winmm waveIn entry points missing -- no "
                        "capture\n");
        p_Open = 0;
        return 0;
    }
    return 1;
}

/* ASCII case-insensitive substring test. SUBSTRING and not equality on
   purpose: waveInGetDevCapsA truncates a device name to 31 characters plus a
   NUL, so the string a launcher shows a player and the string this program
   sees are routinely different lengths, and an exact match would refuse the
   device the player picked out of the launcher's own list. */
int contains_ci(const char *hay, const char *needle)
{
    if (!needle || !*needle) return 1;
    for (const char *h = hay; *h; ++h) {
        const char *a = h, *b = needle;
        for (;;) {
            if (!*b) return 1;
            if (!*a) break;
            char x = *a, y = *b;
            if (x >= 'A' && x <= 'Z') x = (char)(x - 'A' + 'a');
            if (y >= 'A' && y <= 'Z') y = (char)(y - 'A' + 'a');
            if (x != y) break;
            ++a; ++b;
        }
    }
    return 0;
}

/* ASCII case-insensitive WHOLE-STRING compare, for the reserved words "auto",
   "default" and "system". Separate from contains_ci on purpose: a substring
   test would make a device literally named "Default Input" mean the keyword,
   which is a collision a player has no way to debug. */
int equals_ci(const char *a, const char *b)
{
    for (;; ++a, ++b) {
        char x = *a, y = *b;
        if (x >= 'A' && x <= 'Z') x = (char)(x - 'A' + 'a');
        if (y >= 'A' && y <= 'Z') y = (char)(y - 'A' + 'a');
        if (x != y) return 0;
        if (!x) return 1;
    }
}

/* The one capture format this file uses, in one place, so the scan measures a
   device on exactly the format the live open will ask for. A device that
   refuses the format during the scan would have refused it live too, and is
   reported as such rather than being measured on some other format it happens
   to accept and then handed to an open that fails. */
void voice_format(WAVEFORMATEX *wf)
{
    memset(wf, 0, sizeof *wf);
    wf->wFormatTag = WAVE_FORMAT_PCM;
    wf->nChannels = 1;
    wf->nSamplesPerSec = kCapRate;
    wf->wBitsPerSample = 16;
    wf->nBlockAlign = 2;
    wf->nAvgBytesPerSec = kCapRate * 2;
}

/* Record from an ALREADY-OPEN device for at most `ms` and return the peak
   absolute sample seen (0..32767), or -1 when the recording path could not be
   set up at all. Opens nothing and closes nothing: the caller owns `dev`.

   RETURNS EARLY the moment the peak clears `stop_at`, which is what keeps the
   scan cheap on the machine that has nothing wrong with it -- a live
   microphone answers the question in the first buffer or two and the rest of
   the window is never spent.

   The shape is hal/mic_probe_win.cpp's capture_peak, the one that produced the
   1-versus-470 reading quoted at the top of this file (commit a9f5dcc4, the
   per-device sweep), with two differences: it uses the entry points load_lib
   already resolved rather than resolving its own, and it has the early-out.
   The buffers are the live path's own 20 ms frames, so the device is driven
   during the scan exactly the way it will be driven for real. */
long scan_peak(HWAVEIN dev, int ms, long stop_at)
{
    enum { SBUF = 4 };                 // 4 x 20 ms = 80 ms of ring
    short *buf[SBUF];
    WAVEHDR hdr[SBUF];
    int prepared = 0;
    for (int i = 0; i < SBUF; ++i) {
        buf[i] = (short *)calloc(kCapFrameSamples, sizeof(short));
        if (!buf[i]) break;
        memset(&hdr[i], 0, sizeof hdr[i]);
        hdr[i].lpData = (LPSTR)buf[i];
        hdr[i].dwBufferLength = kCapFrameSamples * sizeof(short);
        if (p_Prepare(dev, &hdr[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
            free(buf[i]);
            buf[i] = 0;
            break;
        }
        p_Add(dev, &hdr[i], sizeof(WAVEHDR));
        ++prepared;
    }
    if (prepared == 0) {
        for (int i = 0; i < SBUF; ++i) if (buf[i]) free(buf[i]);
        return -1;
    }

    p_Start(dev);
    short peak = 0;
    const DWORD start = GetTickCount();
    int next = 0;
    while ((long)peak <= stop_at &&
           (DWORD)(GetTickCount() - start) < (DWORD)ms) {
        WAVEHDR &h = hdr[next];
        if (h.dwFlags & WHDR_DONE) {
            int got = (int)(h.dwBytesRecorded / sizeof(short));
            if (got > kCapFrameSamples) got = kCapFrameSamples;
            for (int i = 0; i < got; ++i) {
                const short v = buf[next][i];
                const short a = v < 0 ? (short)-v : v;
                if (a > peak) peak = a;
            }
            h.dwFlags &= ~WHDR_DONE;
            h.dwBytesRecorded = 0;
            h.dwBufferLength = kCapFrameSamples * sizeof(short);
            p_Add(dev, &h, sizeof(WAVEHDR));
        }
        next = (next + 1) % prepared;
        Sleep(2);
    }

    p_Reset(dev);                      // every queued header comes back DONE
    p_Stop(dev);
    for (int i = 0; i < prepared; ++i) {
        p_Unprepare(dev, &hdr[i], sizeof(WAVEHDR));
        free(buf[i]);
    }
    return (long)peak;
}

/* THE AUTO-PICK. Open every recording device in turn on the voice format,
   measure it, and return the id of the FIRST one that is actually producing
   audio.

   CACHED. The answer is computed once per process and cap_rearm is the only
   thing that drops it, because the only event that justifies paying for the
   scan again is a player changing a setting. The open-retry timer must never
   trigger it, or a box whose microphone is broken would re-scan every device
   every five seconds for the length of the session.

   NEVER HARD-FAILS. No devices, no device that opens, or every device silent
   all end at WAVE_MAPPER -- the Windows default -- with a line saying which of
   those it was. The player gets whatever Windows would have handed him anyway,
   plus a sentence telling him where to look. */
UINT auto_pick()
{
    if (g_auto_done) return g_auto_id;
    g_auto_done = 1;
    g_auto_id = WAVE_MAPPER;

    const UINT n = p_NumDevs();
    if (n == 0) {
        fprintf(stderr, "[voice] auto-pick: this machine reports NO recording "
                        "devices; using the Windows default and expecting "
                        "nothing from it\n");
        return g_auto_id;
    }

    WAVEFORMATEX wf;
    voice_format(&wf);
    fprintf(stderr, "[voice] auto-pick: measuring %u recording device%s at "
                    "%d Hz mono, up to %d ms each, silence floor %d/32767\n",
            n, n == 1 ? "" : "s", (int)kCapRate, (int)kScanWindowMs,
            (int)kSilencePeak);

    const DWORD scan_start = GetTickCount();
    long best_peak = -1;
    int  best_index = -1;
    char best_name[kCapNameBytes];
    best_name[0] = 0;
    UINT measured = 0;

    for (UINT i = 0; i < n; ++i) {
        if ((DWORD)(GetTickCount() - scan_start) >= (DWORD)kScanBudgetMs) {
            fprintf(stderr, "[voice] auto-pick:   device %u and past: NOT "
                            "measured, the %d ms scan budget is spent\n",
                    i, (int)kScanBudgetMs);
            break;
        }

        WAVEINCAPSA c;
        memset(&c, 0, sizeof c);
        char name[kCapNameBytes];
        name[0] = 0;
        if (p_Caps((UINT_PTR)i, &c, sizeof c) == MMSYSERR_NOERROR) {
            strncpy(name, c.szPname, sizeof name - 1);
            name[sizeof name - 1] = 0;
        }

        HWAVEIN dev = 0;
        const MMRESULT r = p_Open(&dev, i, &wf, 0, 0, CALLBACK_NULL);
        if (r != MMSYSERR_NOERROR) {
            fprintf(stderr, "[voice] auto-pick:   device %u '%s': will not "
                            "open at the voice format (mmr %d) -- skipped\n",
                    i, name, (int)r);
            continue;
        }

        const long pk = scan_peak(dev, (int)kScanWindowMs, (long)kSilencePeak);
        p_Close(dev);

        if (pk < 0) {
            fprintf(stderr, "[voice] auto-pick:   device %u '%s': opened, but "
                            "the sample buffers could not be set up -- "
                            "skipped\n", i, name);
            continue;
        }
        ++measured;
        fprintf(stderr, "[voice] auto-pick:   device %u '%s': peak %ld/32767 "
                        "-- %s\n", i, name, pk,
                pk > (long)kSilencePeak ? "LIVE" : "silent");

        if (pk > (long)kSilencePeak) {
            g_auto_id = i;
            fprintf(stderr, "[voice] auto-pick: CHOSE device %u '%s' -- first "
                            "device whose peak (%ld) clears the %d/32767 "
                            "silence floor\n", i, name, pk, (int)kSilencePeak);
            return g_auto_id;
        }
        if (pk > best_peak) {
            best_peak = pk;
            best_index = (int)i;
            strncpy(best_name, name, sizeof best_name - 1);
            best_name[sizeof best_name - 1] = 0;
        }
    }

    if (measured == 0) {
        fprintf(stderr, "[voice] auto-pick: none of the %u recording device%s "
                        "could be opened and measured; falling back to the "
                        "Windows default recording device\n",
                n, n == 1 ? "" : "s");
        return g_auto_id;
    }

    /* Every device that opened was silent. That is not a device-choice problem
       and picking the loudest of several dead microphones would only hide it,
       so the fallback is the Windows default and the line says, in words a
       player can act on, what to go and look at. */
    fprintf(stderr, "[voice] auto-pick: ALL %u measured recording device%s "
                    "read SILENT (loudest was device %d '%s' at %ld/32767, "
                    "floor %d). Falling back to the Windows default recording "
                    "device. CHECK WINDOWS SOUND SETTINGS -- the microphone's "
                    "own mute switch, Settings > System > Sound > Input > the "
                    "device's input level slider, and Settings > Privacy & "
                    "security > Microphone. Voice stays silent until one of "
                    "those changes.\n",
            measured, measured == 1 ? "" : "s", best_index,
            best_name[0] ? best_name : "(unnamed)", best_peak,
            (int)kSilencePeak);
    return g_auto_id;
}

/* Which device id the settings ask for, in the order hal/voice_chat.h states:
   VoiceMicIndex verbatim, then the reserved names, then auto-pick, then a
   substring match. WAVE_MAPPER with one line on stderr when a name matches
   nothing -- a player whose headset is unplugged gets the default microphone
   and a sentence saying so, rather than a silent channel he has to debug. */
UINT resolve_device(const char *name, int index)
{
    /* THE MANUAL OVERRIDE, and it is verbatim on purpose. An explicit index is
       a player -- or the launcher's device picker -- naming ONE device. Second
       guessing it with a scan or a name match would make the picker's own
       choice unreproducible, and an index that will not open has to fail
       visibly through the normal open path rather than being quietly swapped
       for something that does. */
    if (index >= 0) {
        WAVEINCAPSA c;
        memset(&c, 0, sizeof c);
        const int ok = p_Caps((UINT_PTR)index, &c, sizeof c) == MMSYSERR_NOERROR;
        fprintf(stderr, "[voice] VoiceMicIndex %d selects device %d '%s' -- "
                        "used verbatim, no auto-pick scan and no name match\n",
                index, index, ok ? c.szPname : "(no such recording device)");
        return (UINT)index;
    }

    /* "" and "auto" are the same answer, and it is the default one. */
    if (!name || !*name || equals_ci(name, "auto")) return auto_pick();

    /* The other reserved word: the player asking for whatever Windows calls
       default, explicitly, with no scan. */
    if (equals_ci(name, "default") || equals_ci(name, "system")) {
        fprintf(stderr, "[voice] VoiceMicDevice '%s' -- the Windows default "
                        "recording device, no auto-pick scan\n", name);
        return WAVE_MAPPER;
    }

    const UINT n = p_NumDevs();
    for (UINT i = 0; i < n; ++i) {
        WAVEINCAPSA c;
        memset(&c, 0, sizeof c);
        if (p_Caps(i, &c, sizeof c) != MMSYSERR_NOERROR) continue;
        if (contains_ci(c.szPname, name)) {
            fprintf(stderr, "[voice] mic device %u '%s' matches "
                            "VoiceMicDevice '%s' -- used as named, no "
                            "auto-pick scan\n", i, c.szPname, name);
            return i;
        }
    }
    fprintf(stderr, "[voice] no recording device matches VoiceMicDevice "
                    "'%s' (%u on this machine); falling back to the system "
                    "default\n", name, n);
    return WAVE_MAPPER;
}

/* Latch a failure against the name that produced it. One line has already
   been printed by whichever arm called this and nothing is printed here, so a
   retry after the caller's backoff is silent too and the log carries a given
   failure once per name rather than once per frame. */
void cap_fail(const char *name, int index)
{
    g_fail = 1;
    g_fail_index = index;
    strncpy(g_fail_name, name ? name : "", sizeof g_fail_name - 1);
    g_fail_name[sizeof g_fail_name - 1] = 0;
}

void free_ring()
{
    for (int i = 0; i < NBUF; ++i) {
        if (g_buf[i]) { free(g_buf[i]); g_buf[i] = 0; }
    }
}

#endif  /* _WIN32 */

}  // namespace

#if defined(_WIN32)

int cap_open(const char *device_name, int device_index)
{
    const char *want = device_name ? device_name : "";
    const int want_ix = device_index < 0 ? -1 : device_index;
    if (g_open) {
        // same device, no-op
        if (want_ix == g_open_index && strcmp(want, g_open_name) == 0) return 1;
        cap_close();                                    // a swap is a reopen
    }
    /* Already known to fail for exactly this name AND index: silent, and not
       one winmm call -- not the enumeration resolve_device would do, and not
       the auto-pick scan either. */
    if (g_fail && want_ix == g_fail_index && strcmp(want, g_fail_name) == 0)
        return 0;
    if (!load_lib()) { cap_fail(want, want_ix); return 0; }

    WAVEFORMATEX wf;
    voice_format(&wf);

    /* SM64DS_VOICE_NO_DEVICE=1 refuses the open as if the machine had no
       recording hardware at all. It exists so the latch above can be PROVEN on
       a box that does have a microphone, without the proof taking it: with it
       set, this function walks the whole real path down to here and then
       fails, and the log shows exactly what a player with no device sees. */
    static int no_dev = -1;
    if (no_dev < 0) no_dev = getenv("SM64DS_VOICE_NO_DEVICE") ? 1 : 0;

    const UINT id = resolve_device(want, want_ix);
    if (no_dev ||
        p_Open(&g_dev, id, &wf, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        fprintf(stderr, "[voice] waveInOpen failed at %d Hz mono on device %d "
                        "-- no capture until the device or the setting "
                        "changes\n", (int)kCapRate,
                id == WAVE_MAPPER ? -1 : (int)id);
        g_dev = 0;
        cap_fail(want, want_ix);
        return 0;
    }
    for (int i = 0; i < NBUF; ++i) {
        g_buf[i] = (short *)calloc(kCapFrameSamples, sizeof(short));
        if (!g_buf[i]) {
            free_ring(); p_Close(g_dev); g_dev = 0;
            cap_fail(want, want_ix); return 0;
        }
        memset(&g_hdr[i], 0, sizeof g_hdr[i]);
        g_hdr[i].lpData = (LPSTR)g_buf[i];
        g_hdr[i].dwBufferLength = kCapFrameSamples * sizeof(short);
        p_Prepare(g_dev, &g_hdr[i], sizeof(WAVEHDR));
        p_Add(g_dev, &g_hdr[i], sizeof(WAVEHDR));
    }
    g_next = 0;
    p_Start(g_dev);
    g_open = 1;
    g_fail = 0;
    g_open_index = want_ix;
    strncpy(g_open_name, want, sizeof g_open_name - 1);
    g_open_name[sizeof g_open_name - 1] = 0;
    {
        /* The name this line prints is the RESOLVED one, read back off the
           device that actually opened, not the setting that asked for it. The
           setting is "" on the auto-pick path, and a line saying the capture
           device is "" is exactly the line that made this bug invisible the
           first time. */
        WAVEINCAPSA c;
        memset(&c, 0, sizeof c);
        const int named = id != WAVE_MAPPER &&
                          p_Caps((UINT_PTR)id, &c, sizeof c) == MMSYSERR_NOERROR;
        fprintf(stderr, "[voice] capture open: %d Hz mono, %d x %d samples "
                        "(%d ms), device %d '%s' (asked for %s)\n",
                (int)kCapRate, NBUF, (int)kCapFrameSamples, NBUF * 20,
                id == WAVE_MAPPER ? -1 : (int)id,
                named ? c.szPname : "(the Windows default device)",
                want_ix >= 0 ? "an explicit VoiceMicIndex"
                             : (want[0] ? want : "auto-pick"));
    }
    return 1;
}

void cap_close()
{
    if (!g_open) return;
    p_Reset(g_dev);                 // every queued header comes back DONE
    p_Stop(g_dev);
    for (int i = 0; i < NBUF; ++i) p_Unprepare(g_dev, &g_hdr[i], sizeof(WAVEHDR));
    p_Close(g_dev);
    g_dev = 0;
    free_ring();
    g_open = 0;
    g_open_index = -1;
    g_open_name[0] = '\0';
    fprintf(stderr, "[voice] capture closed\n");
}

int cap_is_open() { return g_open; }

void cap_rearm()
{
    g_fail = 0;
    g_fail_index = -1;
    g_fail_name[0] = 0;
    /* And the cached auto-pick, for the reason hal/voice_chat.h gives: a
       player action is the one event that justifies paying for the scan again.
       The five-second open retry does NOT come through here, so a box with a
       broken microphone never re-scans. */
    g_auto_done = 0;
    g_auto_id = WAVE_MAPPER;
}

int cap_read_frame(short *out)
{
    if (!g_open || !out) return 0;
    WAVEHDR &h = g_hdr[g_next];
    if (!(h.dwFlags & WHDR_DONE)) return 0;

    /* A header can come back short -- waveInReset returns partial buffers --
       so the tail is zeroed rather than left holding the previous pass's
       audio, which would be an audible repeat rather than a gap. */
    int got = (int)(h.dwBytesRecorded / sizeof(short));
    if (got > kCapFrameSamples) got = kCapFrameSamples;
    memcpy(out, g_buf[g_next], (size_t)got * sizeof(short));
    if (got < kCapFrameSamples)
        memset(out + got, 0, (size_t)(kCapFrameSamples - got) * sizeof(short));

    h.dwFlags &= ~WHDR_DONE;
    h.dwBytesRecorded = 0;
    h.dwBufferLength = kCapFrameSamples * sizeof(short);
    p_Add(g_dev, &h, sizeof(WAVEHDR));
    g_next = (g_next + 1) % NBUF;
    return 1;
}

int cap_enumerate(char names[][kCapNameBytes], int max)
{
    if (!load_lib()) return 0;
    const int n = (int)p_NumDevs();
    for (int i = 0; i < n && i < max; ++i) {
        WAVEINCAPSA c;
        memset(&c, 0, sizeof c);
        names[i][0] = '\0';
        if (p_Caps((UINT_PTR)i, &c, sizeof c) != MMSYSERR_NOERROR) continue;
        strncpy(names[i], c.szPname, kCapNameBytes - 1);
        names[i][kCapNameBytes - 1] = '\0';
    }
    return n;
}

/* SM64DS_VOICE_MIC_PROBE=1. Opens the system default recording device with
   the exact WAVEFORMATEX voice capture uses -- same rate, same 1 channel,
   same 16-bit PCM -- independent of cap_open/g_open above so a probe run
   never disturbs a session that happens to be live. Prints the device name,
   the format requested, and SUCCESS or the waveInOpen error code; on success
   it samples three seconds of real input and prints the peak sample. */
void cap_probe()
{
    fprintf(stderr, "[voice-probe] requesting %d Hz, 1 channel, 16-bit PCM\n",
            (int)kCapRate);
    if (!load_lib()) {
        fprintf(stderr, "[voice-probe] FAILED: winmm unavailable\n");
        return;
    }
    const UINT id = resolve_device("");
    WAVEINCAPSA caps;
    memset(&caps, 0, sizeof caps);
    const char *devname = p_Caps(id, &caps, sizeof caps) == MMSYSERR_NOERROR
                               ? caps.szPname : "(unknown)";
    fprintf(stderr, "[voice-probe] device: %s\n", devname);

    WAVEFORMATEX wf;
    memset(&wf, 0, sizeof wf);
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 1;
    wf.nSamplesPerSec = kCapRate;
    wf.wBitsPerSample = 16;
    wf.nBlockAlign = 2;
    wf.nAvgBytesPerSec = kCapRate * 2;

    HWAVEIN dev;
    const MMRESULT mr = p_Open(&dev, id, &wf, 0, 0, CALLBACK_NULL);
    if (mr != MMSYSERR_NOERROR) {
        fprintf(stderr, "[voice-probe] FAILED: waveInOpen error %d\n", (int)mr);
        return;
    }
    fprintf(stderr, "[voice-probe] SUCCESS\n");

    enum { PBUF = 4 };
    short *buf[PBUF];
    WAVEHDR hdr[PBUF];
    for (int i = 0; i < PBUF; ++i) {
        buf[i] = (short *)calloc(kCapFrameSamples, sizeof(short));
        memset(&hdr[i], 0, sizeof hdr[i]);
        hdr[i].lpData = (LPSTR)buf[i];
        hdr[i].dwBufferLength = kCapFrameSamples * sizeof(short);
        p_Prepare(dev, &hdr[i], sizeof(WAVEHDR));
        p_Add(dev, &hdr[i], sizeof(WAVEHDR));
    }
    p_Start(dev);

    short peak = 0;
    const DWORD until = GetTickCount() + 3000;
    int next = 0;
    while (GetTickCount() < until) {
        WAVEHDR &h = hdr[next];
        if (h.dwFlags & WHDR_DONE) {
            const int n = (int)(h.dwBytesRecorded / sizeof(short));
            for (int i = 0; i < n; ++i) {
                const short v = buf[next][i];
                const short a = v < 0 ? (short)-v : v;
                if (a > peak) peak = a;
            }
            h.dwFlags &= ~WHDR_DONE;
            h.dwBytesRecorded = 0;
            h.dwBufferLength = kCapFrameSamples * sizeof(short);
            p_Add(dev, &h, sizeof(WAVEHDR));
            next = (next + 1) % PBUF;
        }
        Sleep(10);
    }

    p_Reset(dev);
    p_Stop(dev);
    for (int i = 0; i < PBUF; ++i) {
        p_Unprepare(dev, &hdr[i], sizeof(WAVEHDR));
        free(buf[i]);
    }
    p_Close(dev);
    fprintf(stderr, "[voice-probe] peak input level: %d / 32767\n", (int)peak);
}

#else   /* not _WIN32 */

int cap_open(const char *, int) { return 0; }
void cap_close() {}
int cap_is_open() { return 0; }
void cap_rearm() {}
int cap_read_frame(short *) { return 0; }
int cap_enumerate(char[][kCapNameBytes], int) { return 0; }
void cap_probe() {}

#endif

}  // namespace voice
}  // namespace port
