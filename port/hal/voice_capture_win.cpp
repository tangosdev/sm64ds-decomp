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
 * without opening anything.
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

/* Which device id the name asks for. WAVE_MAPPER (the system default) for an
   empty name, and WAVE_MAPPER with one line on stderr when a name matches
   nothing -- a player whose headset is unplugged gets the built-in microphone
   and a sentence saying so, rather than a silent channel he has to debug. */
UINT resolve_device(const char *name)
{
    if (!name || !*name) return WAVE_MAPPER;
    const UINT n = p_NumDevs();
    for (UINT i = 0; i < n; ++i) {
        WAVEINCAPSA c;
        memset(&c, 0, sizeof c);
        if (p_Caps(i, &c, sizeof c) != MMSYSERR_NOERROR) continue;
        if (contains_ci(c.szPname, name)) {
            fprintf(stderr, "[voice] mic device %u '%s' matches "
                            "VoiceMicDevice '%s'\n", i, c.szPname, name);
            return i;
        }
    }
    fprintf(stderr, "[voice] no recording device matches VoiceMicDevice "
                    "'%s' (%u on this machine); falling back to the system "
                    "default\n", name, n);
    return WAVE_MAPPER;
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

int cap_open(const char *device_name)
{
    const char *want = device_name ? device_name : "";
    if (g_open) {
        if (strcmp(want, g_open_name) == 0) return 1;   // same device, no-op
        cap_close();                                    // a swap is a reopen
    }
    if (!load_lib()) return 0;

    WAVEFORMATEX wf;
    memset(&wf, 0, sizeof wf);
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 1;
    wf.nSamplesPerSec = kCapRate;
    wf.wBitsPerSample = 16;
    wf.nBlockAlign = 2;
    wf.nAvgBytesPerSec = kCapRate * 2;

    const UINT id = resolve_device(want);
    if (p_Open(&g_dev, id, &wf, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        fprintf(stderr, "[voice] waveInOpen failed at %d Hz mono -- no "
                        "capture this session\n", (int)kCapRate);
        g_dev = 0;
        return 0;
    }
    for (int i = 0; i < NBUF; ++i) {
        g_buf[i] = (short *)calloc(kCapFrameSamples, sizeof(short));
        if (!g_buf[i]) { free_ring(); p_Close(g_dev); g_dev = 0; return 0; }
        memset(&g_hdr[i], 0, sizeof g_hdr[i]);
        g_hdr[i].lpData = (LPSTR)g_buf[i];
        g_hdr[i].dwBufferLength = kCapFrameSamples * sizeof(short);
        p_Prepare(g_dev, &g_hdr[i], sizeof(WAVEHDR));
        p_Add(g_dev, &g_hdr[i], sizeof(WAVEHDR));
    }
    g_next = 0;
    p_Start(g_dev);
    g_open = 1;
    strncpy(g_open_name, want, sizeof g_open_name - 1);
    g_open_name[sizeof g_open_name - 1] = '\0';
    fprintf(stderr, "[voice] capture open: %d Hz mono, %d x %d samples "
                    "(%d ms), device '%s'\n", (int)kCapRate, NBUF,
            (int)kCapFrameSamples, NBUF * 20,
            g_open_name[0] ? g_open_name : "(system default)");
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
    g_open_name[0] = '\0';
    fprintf(stderr, "[voice] capture closed\n");
}

int cap_is_open() { return g_open; }

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

#else   /* not _WIN32 */

int cap_open(const char *) { return 0; }
void cap_close() {}
int cap_is_open() { return 0; }
int cap_read_frame(short *) { return 0; }
int cap_enumerate(char[][kCapNameBytes], int) { return 0; }

#endif

}  // namespace voice
}  // namespace port
