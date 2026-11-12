/* smoke_settings_live: the in-game settings engine (hal/settings_live.h) proved
   on the file alone, no ROM and no window. Run menu1, lane SETLIVE1.

   host_settings.cpp reads the file once per process, so every case is its own
   process, the way smoke_settings.cpp does it: the parent makes a working
   directory, writes (or does not write) a settings.json, and runs itself with
   a case name. What is pinned:

     defaults   no file: the new keys read their defaults, and the stick
                sweep through the default remap, digital threshold and MP
                run point is IDENTICAL to the constants the game used
                before this lane (7849 dead zone, 12000 digital, 0.87).
     setwrite   every setting set through sl_set and flushed: the file holds
                correctly TYPED values (bools true/false, ints as ints,
                CameraMode / RunMode quoted lowercase, KeyRun beside
                RunButtonKey), a key this program never heard of survives,
                and the launcher's own parser rules are met.
     readback   the file the case above wrote, read by a fresh process: every
                value comes back (the restart row).
     debounce   five sets inside half a second write once at once and the
                last value lands after the half second.
     livereload the file rewritten from outside, host_settings_poll sees it
                and the control generation moves.
     capture    the rebind rules: reserved keys and conflicts are refused with
                a reason, a free key binds, escape cancels, a pad button binds.
     remap      the run-threshold curve at 70 percent: monotonic, the
                player's point lands on the ROM's 0xe47, below stays below.
*/
#include "hal/host_settings.h"
#include "hal/settings_live.h"

#include <direct.h>
#include <math.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

static int fails;

static void check(int cond, const char *what)
{
    if (cond) return;
    ++fails;
    fprintf(stderr, "  FAIL %s\n", what);
}
static void check_eq(long long got, long long want, const char *what)
{
    if (got == want) return;
    ++fails;
    fprintf(stderr, "  FAIL %s: got %lld want %lld\n", what, got, want);
}

static char g_buf[65536];
static const char *slurp(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) return "";
    size_t n = fread(g_buf, 1, sizeof g_buf - 1, f);
    fclose(f);
    g_buf[n] = '\0';
    return g_buf;
}
static int has(const char *text, const char *needle) { return strstr(text, needle) != 0; }
static void check_has(const char *text, const char *needle)
{
    if (has(text, needle)) return;
    ++fails;
    fprintf(stderr, "  FAIL settings.json lacks: %s\n", needle);
}

static int write_file(const char *path, const char *text)
{
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    fputs(text, f);
    fclose(f);
    return 1;
}

/* the formula the game used before this lane, verbatim */
static int old_mag(double len)
{
    const int DEAD = 7849, FULL = 32767;
    if (len > FULL) len = FULL;
    if (!(len > DEAD)) return -1;
    int mag = (int)(4096.0 * (len - DEAD) / (FULL - DEAD));
    if (mag > 0x1000) mag = 0x1000;
    if (mag < 0) mag = 0;
    return mag;
}
static int new_mag(double len)
{
    const int DEAD = host_setting_stick_deadzone_raw();
    const int FULL = 32767;
    if (len > FULL) len = FULL;
    if (!(len > DEAD)) return -1;
    int mag = (int)(4096.0 * (len - DEAD) / (FULL - DEAD));
    if (mag > 0x1000) mag = 0x1000;
    if (mag < 0) mag = 0;
    return host_setting_run_remap(mag);
}

/* a thread that takes the named settings mutex and holds it for `ms` */
static char g_lock_name[96];
static DWORD WINAPI hold_thread(void *arg)
{
    HANDLE m = CreateMutexA(0, FALSE, g_lock_name);
    WaitForSingleObject(m, 1000);
    Sleep((DWORD)(size_t)arg);
    ReleaseMutex(m);
    CloseHandle(m);
    return 0;
}

static int child(const char *which)
{
    if (!strcmp(which, "defaults")) {
        check_eq(host_setting_stick_deadzone_raw(), 7849, "dead zone raw");
        check_eq(host_setting_stick_deadzone_pct(), 24, "dead zone pct");
        check_eq(host_setting_stick_digital(), 12000, "digital threshold");
        check_eq(host_setting_run_threshold_pct(), 89, "run threshold pct");
        check(host_setting_run_threshold_is_default(), "run threshold is default");
        check(host_setting_run_norm() == 0.87, "MP run point 0.87");
        check_eq(host_setting_camera_sensitivity(), 100, "camera sens");
        check_eq(host_setting_mouse_sensitivity(), 100, "mouse sens");
        check_eq(host_setting_invert_camera_y(), 0, "invert y");
        /* the stick sweep: every length 0..32767, every angle class */
        long long diffs = 0, n = 0;
        for (int len = 0; len <= 32767; ++len) {
            ++n;
            if (old_mag(len) != new_mag(len)) ++diffs;
        }
        for (double len = 0.0; len < 40000.0; len += 0.37) {
            ++n;
            if (old_mag(len) != new_mag(len)) ++diffs;
        }
        check_eq(diffs, 0, "stick sweep identical to the old formula");
        printf("  sweep: %lld stick lengths, %lld differ\n", n, diffs);
        /* the camera tops at the defaults */
        check_eq(400 * host_setting_camera_sensitivity() / 100, 400, "cam top");
        check_eq(host_settings_ctl_gen() > 0, 1, "ctl gen");
        return fails;
    }
    if (!strcmp(which, "setwrite")) {
        for (int i = 0; i < SL_COUNT; ++i) check(sl_info(i) != 0, "info");
        check_eq(sl_set(SL_KEY_BASE + HOST_KEY_JUMP, 0x4a), 1, "set jump");
        check_eq(sl_set(SL_KEY_BASE + HOST_KEY_RUN, 0x51), 1, "set run key");
        check_eq(sl_set(SL_PAD_BASE + HOST_PAD_JUMP, 0x0100), 1, "set pad jump");
        check_eq(sl_set(SL_PAD_BASE + HOST_PAD_RUN, 0x0200), 1, "set pad run");
        check_eq(sl_set(SL_RUN_MODE, 2), 1, "run mode");
        check_eq(sl_set(SL_STICK_DEADZONE, 10), 1, "dead zone");
        check_eq(sl_set(SL_RUN_THRESHOLD, 70), 1, "run threshold");
        check_eq(sl_set(SL_CAMERA_MODE, 1), 1, "camera mode");
        check_eq(sl_set(SL_SWAP_CAMERA_TURN, 1), 1, "swap");
        check_eq(sl_set(SL_INVERT_CAMERA_Y, 1), 1, "invert");
        check_eq(sl_set(SL_CAMERA_SENSITIVITY, 150), 1, "cam sens");
        check_eq(sl_set(SL_MOUSE_SENSITIVITY, 60), 1, "mouse sens");
        check_eq(sl_set(SL_MOUSE_CAPTURE, 1), 1, "mouse capture");
        check_eq(sl_set(SL_VOLUME, 33), 1, "volume");
        check_eq(sl_set(SL_MINIMAP_SCALE, 150), 1, "minimap");
        check_eq(sl_get(SL_CHAT_SCALE), 100, "chat size starts at 100");
        check_eq(sl_set(SL_CHAT_SCALE, 150), 1, "chat size");
        check_eq(sl_set(SL_CHAT_SCALE, 130), 1, "chat size between stops snaps");
        check_eq(sl_get(SL_CHAT_SCALE), 125, "130 snapped to 125");
        check_eq(sl_set(SL_CHAT_SCALE, 150), 1, "chat size again");
        check_eq(sl_set(SL_CHAT_SCALE, 50), 0, "chat size under 75 refused");
        check_eq(sl_set(SL_CHAT_SCALE, 300), 0, "chat size over 200 refused");
        /* refusals */
        check_eq(sl_set(SL_STICK_DEADZONE, 51), 0, "dead zone out of range");
        check_eq(sl_set(SL_RUN_THRESHOLD, 10), 0, "run threshold out of range");
        check_eq(sl_set(SL_CAMERA_MODE, 4), 0, "camera mode out of range");   /* HYBRIDCAM1: 3 is hybrid */
        check_eq(sl_set(SL_CAMERA_SENSITIVITY, 5), 0, "cam sens out of range");
        check_eq(sl_set(SL_IMPROVED_MINIMAP, 0), 0, "improved minimap is read only");
        check_eq(sl_flush(), 1, "flush");
        /* live values */
        check_eq(sl_get(SL_RUN_MODE), 2, "get run mode");
        check_eq(host_setting_stick_deadzone_raw(), 3276, "dead zone raw at 10 percent");
        check_eq(host_setting_stick_digital(), 12000LL * 3276 / 7849, "digital scales");
        check_eq(sl_get(SL_MINIMAP_SCALE), 150, "get minimap");
        check(sl_applicable(SL_CAMERA_SENSITIVITY), "sens applies in freecam");
        sl_set(SL_CAMERA_MODE, 2);
        check(!sl_applicable(SL_CAMERA_SENSITIVITY), "sens does not apply in ds");
        check(!sl_applicable(SL_INVERT_CAMERA_Y), "invert does not apply in ds");
        sl_set(SL_CAMERA_MODE, 1);
        sl_flush();
        const char *t = slurp("settings.json");
        check_has(t, "\"KeyJump\": 74");
        check_has(t, "\"KeyRun\": 81");
        check_has(t, "\"RunButtonKey\": 81");
        check_has(t, "\"PadJump\": 256");
        check_has(t, "\"PadRun\": 512");
        check_has(t, "\"RunButtonPad\": 512");
        check_has(t, "\"RunMode\": \"auto\"");
        check_has(t, "\"StickDeadzone\": 10");
        check_has(t, "\"RunThreshold\": 70");
        check_has(t, "\"CameraMode\": \"freecam\"");
        check_has(t, "\"SwapCameraTurnDirection\": true");
        check_has(t, "\"InvertCameraY\": true");
        check_has(t, "\"CameraSensitivity\": 150");
        check_has(t, "\"MouseSensitivity\": 60");
        check_has(t, "\"MouseCapture\": true");
        check_has(t, "\"Volume\": 33");
        check_has(t, "\"MinimapScale\": 1.5");
        check_has(t, "\"ChatScale\": 150");
        check_has(t, "\"LauncherOnlyKey\": \"keep me\"");
        check_has(t, "\"MinigameGap\": true");
        /* the typed-value rules the launcher's parser has: nothing numeric
           where it wants a bool */
        check(!has(t, "\"SwapCameraTurnDirection\": 1"), "swap not written as a number");
        check(!has(t, "\"MinigameGap\": 1"), "gap not mangled");
        printf("---- settings.json after the sets ----\n%s\n", t);
        return fails;
    }
    if (!strcmp(which, "readback")) {
        check_eq(sl_get(SL_KEY_BASE + HOST_KEY_JUMP), 0x4a, "jump");
        check_eq(sl_get(SL_KEY_BASE + HOST_KEY_RUN), 0x51, "run key");
        check_eq(host_setting_run_key(), 0x51, "run key accessor");
        check_eq(sl_get(SL_PAD_BASE + HOST_PAD_JUMP), 0x0100, "pad jump");
        check_eq(sl_get(SL_PAD_BASE + HOST_PAD_RUN), 0x0200, "pad run");
        check_eq(sl_get(SL_RUN_MODE), 2, "run mode");
        check_eq(sl_get(SL_STICK_DEADZONE), 10, "dead zone");
        check_eq(sl_get(SL_RUN_THRESHOLD), 70, "run threshold");
        check_eq(sl_get(SL_CAMERA_MODE), 1, "camera mode");
        check_eq(sl_get(SL_SWAP_CAMERA_TURN), 1, "swap");
        check_eq(sl_get(SL_INVERT_CAMERA_Y), 1, "invert");
        check_eq(sl_get(SL_CAMERA_SENSITIVITY), 150, "cam sens");
        check_eq(sl_get(SL_MOUSE_SENSITIVITY), 60, "mouse sens");
        check_eq(sl_get(SL_MOUSE_CAPTURE), 1, "mouse capture");
        check_eq(sl_get(SL_VOLUME), 33, "volume");
        check_eq(sl_get(SL_MINIMAP_SCALE), 150, "minimap");
        check_eq(sl_get(SL_CHAT_SCALE), 150, "chat size");
        return fails;
    }
    if (!strcmp(which, "debounce")) {
        sl_set(SL_CAMERA_SENSITIVITY, 101);       /* first: written at once */
        check_has(slurp("settings.json"), "\"CameraSensitivity\": 101");
        for (int v = 102; v <= 106; ++v) sl_set(SL_CAMERA_SENSITIVITY, v);
        check_has(slurp("settings.json"), "\"CameraSensitivity\": 101");   /* still waiting */
        check_eq(host_settings_save_pending(), 1, "pending");
        Sleep(650);
        host_settings_save_tick();
        check_has(slurp("settings.json"), "\"CameraSensitivity\": 106");
        check_eq(host_settings_save_pending(), 0, "nothing pending");
        return fails;
    }
    if (!strcmp(which, "livereload")) {
        const int g0 = host_settings_ctl_gen();
        check_eq(sl_get(SL_CAMERA_SENSITIVITY), 100, "start");
        Sleep(50);
        write_file("settings.json",
            "{\n  \"CameraSensitivity\": 220,\n  \"InvertCameraY\": true,\n"
            "  \"CameraMode\": \"analog\",\n  \"KeyJump\": 70,\n"
            "  \"StickDeadzone\": 30,\n  \"Volume\": 12,\n  \"RunMode\": \"button\",\n"
            "  \"ChatScale\": 200\n}\n");
        int moved = 0;
        for (int i = 0; i < 40; ++i) moved |= host_settings_poll();
        check(moved, "poll saw the launcher's edit");
        check(host_settings_ctl_gen() != g0, "ctl gen moved");
        check_eq(sl_get(SL_CAMERA_SENSITIVITY), 220, "cam sens live");
        check_eq(sl_get(SL_INVERT_CAMERA_Y), 1, "invert live");
        check_eq(sl_get(SL_CAMERA_MODE), 0, "camera live");
        check_eq(sl_get(SL_KEY_BASE + HOST_KEY_JUMP), 70, "jump live");
        check_eq(sl_get(SL_STICK_DEADZONE), 30, "dead zone live");
        check_eq(sl_get(SL_RUN_MODE), 0, "run mode live");
        check_eq(sl_get(SL_VOLUME), 12, "volume live");
        check_eq(sl_get(SL_CHAT_SCALE), 200, "chat size live");
        check_eq(host_settings_cam_gen() > 1, 1, "cam gen moved");
        /* and the game's own write is not seen as a change back */
        const int g1 = host_settings_ctl_gen();
        sl_set(SL_MOUSE_SENSITIVITY, 80);
        sl_flush();
        for (int i = 0; i < 70; ++i) host_settings_poll();
        check_eq(sl_get(SL_CAMERA_SENSITIVITY), 220, "own write did not revert");
        check_eq(sl_get(SL_MOUSE_SENSITIVITY), 80, "own write kept");
        (void)g1;
        return fails;
    }
    if (!strcmp(which, "capture")) {
        check_eq(sl_capture_start(SL_KEY_BASE + HOST_KEY_JUMP), 1, "start");
        check_eq(sl_capture_state(), SL_CAP_WAITING, "waiting");
        check_eq(sl_capture_take_arm(), 1, "arm requested");
        check_eq(sl_capture_offer_pad(0x0100), 0, "a key row ignores the pad");
        check_eq(sl_capture_offer_key(0x70), 0, "F1 refused");
        check(sl_capture_refused()[0] != 0, "reason given");
        check_eq(sl_capture_offer_key(0x53), 0, "S is the Down key: refused");
        check_eq(sl_capture_offer_key(0x0d), 0, "enter refused for jump");
        check_eq(sl_capture_offer_key(0x25), 0, "arrow refused for jump");
        check_eq(sl_capture_offer_key(0x4a), 1, "J binds");
        check_eq(sl_get(SL_KEY_BASE + HOST_KEY_JUMP), 0x4a, "jump is J");
        check_eq(sl_capture_state(), SL_CAP_BOUND, "bound");
        check_eq(sl_capture_state(), SL_CAP_IDLE, "then idle");
        /* start: enter is fine for Start; escape cancels */
        sl_capture_start(SL_KEY_BASE + HOST_KEY_START);
        check_eq(sl_capture_offer_key(0x0d), 1, "enter binds to Start");
        sl_capture_state();
        sl_capture_start(SL_KEY_BASE + HOST_KEY_ATTACK);
        check_eq(sl_capture_offer_key(0x1b), 1, "escape ends it");
        check_eq(sl_capture_state(), SL_CAP_CANCELLED, "cancelled");
        check_eq(sl_get(SL_KEY_BASE + HOST_KEY_ATTACK), 0x58, "attack unchanged");
        /* an arrow slot may take an arrow */
        sl_capture_start(SL_KEY_BASE + HOST_KEY_LEFT_ALT);
        check_eq(sl_capture_offer_key(0x25), 1, "arrow binds to the arrow slot");
        sl_capture_state();
        /* pad */
        sl_capture_start(SL_PAD_BASE + HOST_PAD_JUMP);
        check_eq(sl_capture_offer_key(0x4b), 0, "a pad row ignores keys");
        check_eq(sl_capture_offer_pad(0x0004), 0, "d-pad refused");
        check_eq(sl_capture_offer_pad(0x4000), 0, "X is the run button: refused");
        check_eq(sl_capture_offer_pad(0x0100), 1, "LB binds");
        check_eq(sl_get(SL_PAD_BASE + HOST_PAD_JUMP), 0x0100, "pad jump is LB");
        sl_capture_state();
        /* the old F5 run row: either device */
        sl_capture_start_run();
        check_eq(sl_capture_offer_key(0x51), 1, "run row takes a key");
        check_eq(sl_get(SL_KEY_BASE + HOST_KEY_RUN), 0x51, "run is Q");
        sl_flush();
        const char *t = slurp("settings.json");
        check_has(t, "\"KeyJump\": 74");
        check_has(t, "\"KeyRun\": 81");
        check_has(t, "\"RunButtonKey\": 81");
        (void)t;   /* KeyStart stayed 13, an unchanged value writes nothing */
        return fails;
    }
    if (!strcmp(which, "fixes")) {
        /* SETFIX1 G4: A and B bind back to a gameplay action, never to Start */
        sl_capture_start(SL_PAD_BASE + HOST_PAD_JUMP);
        check_eq(sl_capture_offer_pad(0x0100), 1, "jump moves to LB");
        sl_capture_state();
        sl_capture_start(SL_PAD_BASE + HOST_PAD_JUMP);
        check_eq(sl_capture_offer_pad(0x1000), 1, "A binds back to Jump");
        check_eq(sl_get(SL_PAD_BASE + HOST_PAD_JUMP), 0x1000, "jump is A");
        sl_capture_state();
        sl_capture_start(SL_PAD_BASE + HOST_PAD_ATTACK);
        check_eq(sl_capture_offer_pad(0x1000), 0, "A is taken by Jump");
        check_eq(sl_capture_offer_pad(0x2000), 1, "B binds to Attack");
        sl_capture_state();
        sl_capture_start(SL_PAD_BASE + HOST_PAD_START);
        check_eq(sl_capture_offer_pad(0x2000), 0, "B refused for Start");
        check_eq(sl_capture_offer_pad(0x0004), 0, "d-pad still refused");
        sl_capture_cancel();
        /* Start keeps one of its two bindings */
        check_eq(sl_set(SL_KEY_BASE + HOST_KEY_START, 0), 1, "Start key can go while the pad has Start");
        check_eq(sl_set(SL_PAD_BASE + HOST_PAD_START, 0), 0, "Start pad cannot go too");
        check(sl_set_refused()[0] != 0, "a reason is given");
        check_eq(sl_get(SL_PAD_BASE + HOST_PAD_START), 0x10, "Start pad stays");
        check_eq(sl_set(SL_KEY_BASE + HOST_KEY_START, 0x0d), 1, "key back");
        check_eq(sl_set(SL_PAD_BASE + HOST_PAD_START, 0), 1, "pad can go while the key has Start");
        check_eq(sl_set(SL_KEY_BASE + HOST_KEY_START, 0), 0, "now the key cannot go");
        check_eq(sl_get(SL_KEY_BASE + HOST_KEY_START), 0x0d, "Start key stays");
        /* the digital walk threshold has a floor */
        check_eq(sl_set(SL_STICK_DEADZONE, 0), 1, "dead zone 0");
        check_eq(host_setting_stick_digital(), 4000, "digital threshold floored at 4000");
        check_eq(sl_set(SL_STICK_DEADZONE, 24), 1, "dead zone 24");
        check_eq(host_setting_stick_digital(), 12000, "digital back to 12000");
        /* SETFIX1 G2: a write that fails keeps the queue and retries on the tick */
        sl_flush();
        SetFileAttributesA("settings.json", FILE_ATTRIBUTE_READONLY);
        sl_set(SL_VOLUME, 71);
        check_eq(sl_flush(), 0, "the write fails on a read-only file");
        check_eq(host_settings_save_pending() > 0, 1, "the failed value stays queued");
        SetFileAttributesA("settings.json", FILE_ATTRIBUTE_NORMAL);
        Sleep(600);
        host_settings_save_tick();
        check_eq(host_settings_save_pending(), 0, "the retry on the next tick lands");
        check_has(slurp("settings.json"), "\"Volume\": 71");
        /* the last good copy follows every good save */
        sl_set(SL_VOLUME, 37);
        sl_flush();
        check_has(slurp("settings.json.lastgood"), "\"Volume\": 37");
        return fails;
    }
    if (!strcmp(which, "mutex")) {
        /* SETFIX1 L1: the save waits for the shared mutex, then proceeds */
        char full[MAX_PATH], name[96];
        GetFullPathNameA("settings.json", MAX_PATH, full, 0);
        host_settings_lock_name(full, name, sizeof name);
        snprintf(g_lock_name, sizeof g_lock_name, "%s", name);
        check(strncmp(name, "Local\\SM64DSPort.settings.", 26) == 0, "mutex name shape");
        printf("  mutex name for %s: %s\n", full, name);
        HANDLE m = CreateMutexA(0, FALSE, name);
        check(m != 0, "mutex created");
        HANDLE th = CreateThread(0, 0, hold_thread, (void *)(size_t)300, 0, 0);
        Sleep(50);
        const DWORD t0 = GetTickCount();
        sl_set(SL_VOLUME, 41);   /* may write at once: the wait can be in here */
        check_eq(sl_flush(), 1, "flush with the lock held elsewhere");
        const DWORD dt = GetTickCount() - t0;
        printf("  flush waited %lu ms behind a 300 ms holder\n", (unsigned long)dt);
        check(dt >= 200, "the save waited for the holder");
        check_has(slurp("settings.json"), "\"Volume\": 41");
        WaitForSingleObject(th, 3000);
        /* a holder that never lets go: the save proceeds after about 500 ms */
        th = CreateThread(0, 0, hold_thread, (void *)(size_t)2500, 0, 0);
        Sleep(50);
        const DWORD t1 = GetTickCount();
        sl_set(SL_VOLUME, 42);
        check_eq(sl_flush(), 1, "flush proceeds after the timeout");
        const DWORD dt2 = GetTickCount() - t1;
        printf("  flush gave up waiting after %lu ms\n", (unsigned long)dt2);
        check(dt2 >= 400 && dt2 < 1500, "short timeout, no hang");
        check_has(slurp("settings.json"), "\"Volume\": 42");
        WaitForSingleObject(th, 5000);
        CloseHandle(m);
        return fails;
    }
    if (!strcmp(which, "remap")) {
        check_eq(sl_set(SL_RUN_THRESHOLD, 70), 1, "70");
        const int T = 70 * 4096 / 100;
        int prev = -1, bad = 0;
        for (int m = 0; m <= 0x1000; ++m) {
            const int o = host_setting_run_remap(m);
            if (o < prev) ++bad;
            prev = o;
            if (m < T && o >= 0xe47) ++bad;
            if (m >= T && o < 0xe47) ++bad;
        }
        check_eq(bad, 0, "monotonic, and the ROM's 0xe47 lands at the player's point");
        check_eq(host_setting_run_remap(0x1000), 0x1000, "full stays full");
        check_eq(host_setting_run_remap(0), 0, "zero stays zero");
        printf("  RunThreshold 70: in -> out\n");
        for (int m = 0; m <= 0x1000; m += 0x200)
            printf("    %4d (%3d%%) -> %4d (%3d%%)\n", m, m * 100 / 4096,
                   host_setting_run_remap(m), host_setting_run_remap(m) * 100 / 4096);
        check_eq(sl_set(SL_RUN_THRESHOLD, 100), 1, "100");
        check(host_setting_run_remap(0x1000) < 0xe47, "100 never runs");
        check_eq(sl_set(SL_RUN_THRESHOLD, 89), 1, "89");
        check(host_setting_run_threshold_is_default(), "89 is the ROM point");
        check_eq(host_setting_run_remap(1234), 1234, "89 is identity");
        return fails;
    }
    fprintf(stderr, "unknown case %s\n", which);
    return 2;
}

static int run_case(const char *exe, const char *dir, const char *which,
                    const char *json, int keep)
{
    char path[MAX_PATH];
    snprintf(path, sizeof path, "%s\\settings.json", dir);
    const int reuse = json == (const char *)1;
    if (!reuse) remove(path);
    if (json && !reuse && !write_file(path, json)) {
        fprintf(stderr, "smoke_settings_live: cannot write %s\n", path);
        return 1;
    }
    char cwd[MAX_PATH];
    if (!_getcwd(cwd, sizeof cwd)) return 1;
    if (_chdir(dir) != 0) return 1;
    const intptr_t rc = _spawnl(_P_WAIT, exe, "smoke_settings_live", which, (char *)0);
    _chdir(cwd);
    if (!keep) remove(path);
    if (rc != 0) {
        fprintf(stderr, "smoke_settings_live: case %s FAILED (rc %d)\n", which, (int)rc);
        return 1;
    }
    printf("smoke_settings_live: %s ok\n", which);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc >= 2) return child(argv[1]);

    char exe[MAX_PATH];
    if (!GetModuleFileNameA(NULL, exe, MAX_PATH)) return 1;
    {
        char beside[MAX_PATH];
        snprintf(beside, sizeof beside, "%s", exe);
        char *slash = strrchr(beside, '\\');
        if (slash) *slash = '\0';
        snprintf(beside + strlen(beside), sizeof beside - strlen(beside), "\\settings.json");
        FILE *f = fopen(beside, "rb");
        if (f) {
            fclose(f);
            fprintf(stderr, "smoke_settings_live: %s exists; move it away and rerun.\n", beside);
            return 1;
        }
    }
    _putenv("SM64DS_ASSET_ROOT=");
    _putenv("SM64DS_INSTANCE=");
    _putenv("SM64DS_VOLUME=");
    _putenv("SM64DS_MINIMAP_SCALE=");
    _putenv("SM64DS_IMPROVED_MINIMAP=");

    char tmp[MAX_PATH], dir[MAX_PATH];
    if (!GetTempPathA(MAX_PATH, tmp)) return 1;
    snprintf(dir, sizeof dir, "%ssm64ds_smoke_settings_live_%lu", tmp,
             (unsigned long)GetCurrentProcessId());
    _mkdir(dir);

    /* a launcher-shaped file: its own keys, bools as bools, a key this
       program has never heard of */
    const char *launcher =
        "{\n"
        "  \"MinigameGap\": true,\n"
        "  \"GapPeek\": false,\n"
        "  \"SwapCameraTurnDirection\": false,\n"
        "  \"CameraMode\": \"ds\",\n"
        "  \"RunMode\": \"analog\",\n"
        "  \"AutoUpdate\": true,\n"
        "  \"LauncherOnlyKey\": \"keep me\",\n"
        "  \"Volume\": 50\n"
        "}\n";

    int bad = 0;
    bad |= run_case(exe, dir, "defaults", 0, 0);
    bad |= run_case(exe, dir, "setwrite", launcher, 1);
    bad |= run_case(exe, dir, "readback", (const char *)1, 0);
    bad |= run_case(exe, dir, "debounce", launcher, 0);
    bad |= run_case(exe, dir, "livereload", launcher, 0);
    bad |= run_case(exe, dir, "capture", launcher, 0);
    bad |= run_case(exe, dir, "remap", launcher, 0);
    bad |= run_case(exe, dir, "fixes", launcher, 0);
    bad |= run_case(exe, dir, "mutex", launcher, 0);
    _rmdir(dir);
    printf(bad ? "smoke_settings_live: FAILED\n" : "smoke_settings_live: all ok\n");
    return bad;
}
