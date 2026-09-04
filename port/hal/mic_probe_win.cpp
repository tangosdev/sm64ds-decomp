/* Microphone-capture probe -- the winmm half. See hal/mic_probe.h for why.
 *
 * MIRRORS hal/voice_capture_win.cpp's device access EXACTLY: winmm.dll is
 * resolved with LoadLibraryA at first use rather than linked as a static
 * import, because the loader resolves static imports BEFORE the TLS callback
 * that claims 0x02000000..0x07ffffff for the hosted DS regions, and a DLL the
 * loader placed in that range takes an address the game needs. The live
 * capture file banks that trap; this one is the same trap with the same shape,
 * so it gets the same answer. It also opens through the same waveInOpen the
 * live path uses, so what it measures is what the real path would hit.
 *
 * IT OPENS AND IMMEDIATELY CLOSES. Each ladder rung is a waveInOpen /
 * waveInClose with no buffers queued and no waveInStart, because the only
 * question is "will the device accept this format", not "what does it sound
 * like". Nothing is recorded and nothing is held.
 */
#include "hal/mic_probe.h"
#include "hal/voice_chat.h"   /* kCapRate: the live format's rate, so the
                                 "current" rung stays honest if it ever moves */

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

#if defined(_WIN32)

namespace {

typedef MMRESULT (WINAPI *pfnInOpen)(HWAVEIN *, UINT, const WAVEFORMATEX *,
                                     DWORD_PTR, DWORD_PTR, DWORD);
typedef MMRESULT (WINAPI *pfnInClose)(HWAVEIN);
typedef UINT     (WINAPI *pfnInNum)(void);
typedef MMRESULT (WINAPI *pfnInCaps)(UINT_PTR, LPWAVEINCAPSA, UINT);
typedef MMRESULT (WINAPI *pfnErrText)(MMRESULT, LPSTR, UINT);

/* Two sinks: mic_probe.log next to the exe, and stderr. A run whose stderr
   nobody captured still leaves the file behind, which is the case that matters
   -- the tester double-clicks the exe, he does not run it from a console. */
FILE *g_log;

void emit(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    if (g_log) {
        va_list ap2;
        va_copy(ap2, ap);
        vfprintf(g_log, fmt, ap2);
        va_end(ap2);
    }
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

/* mic_probe.log beside the running exe. Falls back to the working directory
   if the module path cannot be had, and to stderr-only if neither opens. */
void open_log()
{
    char path[MAX_PATH];
    DWORD n = GetModuleFileNameA(NULL, path, sizeof path);
    if (n > 0 && n < sizeof path) {
        char *slash = strrchr(path, '\\');
        if (slash) {
            slash[1] = '\0';
            strncat(path, "mic_probe.log", sizeof path - strlen(path) - 1);
            g_log = fopen(path, "w");
            if (g_log)
                fprintf(stderr, "[mic-probe] writing %s\n", path);
        }
    }
    if (!g_log) {
        g_log = fopen("mic_probe.log", "w");
        if (g_log)
            fprintf(stderr, "[mic-probe] writing .\\mic_probe.log\n");
        else
            fprintf(stderr, "[mic-probe] could not open a log file; "
                            "stderr only\n");
    }
}

/* A short human name for a winmm MMRESULT, and the interpretation the tester
   and the ladder-builder both need. waveInGetErrorTextA gives Windows' own
   sentence on top of this. */
const char *result_class(MMRESULT r)
{
    switch (r) {
    case MMSYSERR_NOERROR:   return "OK";
    case WAVERR_BADFORMAT:   return "UNSUPPORTED FORMAT";
    case MMSYSERR_ALLOCATED: return "DEVICE IN USE (held exclusively -- another "
                                    "app, or exclusive mode)";
    case MMSYSERR_BADDEVICEID:return "BAD DEVICE ID (no such recording device)";
    case MMSYSERR_NODRIVER:  return "NO DRIVER (no recording device present)";
    case MMSYSERR_NOTENABLED:return "DRIVER NOT ENABLED";
    case MMSYSERR_NOMEM:     return "OUT OF MEMORY";
    case MMSYSERR_INVALPARAM:return "INVALID PARAMETER";
    case MMSYSERR_INVALFLAG: return "INVALID FLAG";
    case MMSYSERR_NOTSUPPORTED:return "NOT SUPPORTED";
    default:                 return "OTHER ERROR";
    }
}

struct Rung { int rate; int channels; const char *note; };

}  // namespace

int mic_probe_run_if_enabled()
{
    const char *flag = getenv("SM64DS_MIC_PROBE");
    if (!flag || strcmp(flag, "1") != 0) return 0;   // inert

    open_log();

    emit("========================================================\n");
    emit("SM64DS microphone capture probe\n");
    emit("What this does: opens the default recording device across a ladder\n");
    emit("of common formats and reports which ones the device accepts. It\n");
    emit("records nothing and changes nothing about the game.\n");
    emit("========================================================\n\n");

    HMODULE lib = LoadLibraryA("winmm.dll");
    if (!lib) {
        emit("winmm.dll could not be loaded -- this machine has no path to\n"
             "the recording API at all. RESULT: no capture possible.\n");
        if (g_log) fclose(g_log);
        return 1;
    }
    pfnInOpen  p_Open  = (pfnInOpen) GetProcAddress(lib, "waveInOpen");
    pfnInClose p_Close = (pfnInClose)GetProcAddress(lib, "waveInClose");
    pfnInNum   p_Num   = (pfnInNum)  GetProcAddress(lib, "waveInGetNumDevs");
    pfnInCaps  p_Caps  = (pfnInCaps) GetProcAddress(lib, "waveInGetDevCapsA");
    pfnErrText p_Err   = (pfnErrText)GetProcAddress(lib, "waveInGetErrorTextA");
    if (!p_Open || !p_Close || !p_Num || !p_Caps) {
        emit("winmm is present but its waveIn entry points are missing.\n"
             "RESULT: no capture possible.\n");
        if (g_log) fclose(g_log);
        return 1;
    }

    /* The device inventory first. This is what SM64DS_VOICE_DEVICES prints,
       repeated here so one log answers both "is the mic seen at all" and
       "which formats does it take". */
    const UINT ndev = p_Num();
    emit("Recording devices Windows reports: %u\n", ndev);
    if (ndev == 0) {
        emit("  (none) -- either there is no microphone, or Windows is hiding\n"
             "  every recording device from this app, which is what the\n"
             "  microphone privacy setting does. Check Settings > Privacy &\n"
             "  security > Microphone, and that 'Let desktop apps access your\n"
             "  microphone' is on.\n");
    }
    for (UINT i = 0; i < ndev; ++i) {
        WAVEINCAPSA c;
        memset(&c, 0, sizeof c);
        if (p_Caps((UINT_PTR)i, &c, sizeof c) != MMSYSERR_NOERROR) {
            emit("  device %u: <caps unavailable>\n", i);
            continue;
        }
        emit("  device %u: '%s'  (channels up to %u, standard-format bits "
             "0x%08lX)\n", i, c.szPname, (unsigned)c.wChannels,
             (unsigned long)c.dwFormats);
    }
    emit("\nThe ladder below opens WAVE_MAPPER, the system DEFAULT capture\n"
         "device -- the same target the live voice path opens.\n\n");

    /* The ladder. All 16-bit PCM. The first rung is the format the live path
       hardcodes today (hal/voice_capture_win.cpp: kCapRate mono 16-bit); the
       rest are the common shapes a real mic is likely to accept when that one
       is refused. */
    const Rung ladder[] = {
        { (int)kCapRate, 1, "the current live format" },
        { 44100,         2, "" },
        { 48000,         2, "" },
        { 48000,         1, "" },
        { 22050,         1, "" },
        { (int)kCapRate, 2, "" },
    };
    const int rungs = (int)(sizeof ladder / sizeof ladder[0]);

    int opened_any = 0;
    int first_error = 0;      // the code the first failing rung returned
    int all_same_error = 1;   // do all failing rungs return the same code?
    int any_badformat = 0;

    for (int i = 0; i < rungs; ++i) {
        const Rung &r = ladder[i];
        WAVEFORMATEX wf;
        memset(&wf, 0, sizeof wf);
        wf.wFormatTag      = WAVE_FORMAT_PCM;
        wf.nChannels       = (WORD)r.channels;
        wf.nSamplesPerSec  = (DWORD)r.rate;
        wf.wBitsPerSample  = 16;
        wf.nBlockAlign     = (WORD)(r.channels * 2);
        wf.nAvgBytesPerSec = (DWORD)(r.rate * r.channels * 2);

        HWAVEIN dev = 0;
        MMRESULT res = p_Open(&dev, WAVE_MAPPER, &wf, 0, 0, CALLBACK_NULL);

        char tail[256];
        tail[0] = '\0';
        if (res != MMSYSERR_NOERROR && p_Err)
            p_Err(res, tail, sizeof tail);

        emit("  %5d Hz %-6s 16-bit : %-52s [mmr %d]%s%s%s\n",
             r.rate, r.channels == 1 ? "mono" : "stereo",
             result_class(res), (int)res,
             tail[0] ? " -- " : "", tail,
             r.note[0] ? "  <- " : "");
        if (r.note[0])
            emit("                                 %s\n", r.note);

        if (res == MMSYSERR_NOERROR) {
            opened_any = 1;
            if (dev) p_Close(dev);
        } else {
            if (res == WAVERR_BADFORMAT) any_badformat = 1;
            if (!first_error) first_error = (int)res;
            else if ((int)res != first_error) all_same_error = 0;
        }
    }

    /* The verdict, in plain terms, because a tester reads this and an agent
       builds the real ladder from it. */
    emit("\n--------------------------------------------------------\n");
    if (opened_any) {
        emit("VERDICT: at least one format opened. The microphone works; the\n"
             "live path fails only because it locks to one format with no\n"
             "fallback. Build the real ladder from the rungs marked OK above.\n");
    } else if (ndev == 0) {
        emit("VERDICT: no recording device was visible, so nothing could open.\n"
             "This is almost always the Windows microphone privacy gate or an\n"
             "unplugged/absent mic -- NOT a format problem. Enable microphone\n"
             "access for desktop apps and run this again.\n");
    } else if (any_badformat && all_same_error == 0) {
        emit("VERDICT: the device exists and some formats were refused as\n"
             "UNSUPPORTED FORMAT while others failed differently. It is a\n"
             "format-support problem; the real path needs the fallback ladder.\n");
    } else if (all_same_error && first_error == MMSYSERR_ALLOCATED) {
        emit("VERDICT: every rung failed with DEVICE IN USE. Another program\n"
             "holds the microphone exclusively (or exclusive mode is on in the\n"
             "device's advanced properties). Close what is using it, or turn\n"
             "off exclusive mode, then run this again.\n");
    } else if (all_same_error) {
        emit("VERDICT: the device exists but EVERY format failed with the same\n"
             "error (mmr %d, %s). Since it is not a per-format refusal, suspect\n"
             "a permission/privacy gate or a driver problem rather than the\n"
             "format. Check the microphone privacy setting first.\n",
             first_error, result_class((MMRESULT)first_error));
    } else {
        emit("VERDICT: the device exists but no format opened, and the failures\n"
             "were mixed. See the per-rung errors above; if UNSUPPORTED FORMAT\n"
             "appears, a fallback ladder helps, otherwise treat it as a\n"
             "permission or driver problem.\n");
    }
    emit("--------------------------------------------------------\n");

    if (g_log) { fclose(g_log); g_log = 0; }
    /* winmm stays loaded; the process is about to exit anyway. */
    return 1;
}

#else   /* not _WIN32 */

int mic_probe_run_if_enabled() { return 0; }

#endif

}  // namespace voice
}  // namespace port
