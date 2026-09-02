/* smoke_settings: the control-binding and camera-mode half of settings.json,
   proved without a window, a pad or a ROM.

   hal/host_settings.cpp reads the file ONCE per process, so every case here
   is its own process: the parent makes an empty working directory, writes
   (or does not write) a settings.json into it, and runs itself again with a
   case name, chdir'd into that directory. The child asks the getters and
   exits non-zero on the first wrong answer, and the parent fails if any child
   does. What is pinned:

     defaults    no file at all: every key and pad default the header
                 promises, RunButtonKey/RunButtonPad agreeing with
                 KeyRun/PadRun, CameraMode analog.
     remap       a file that moves jump, attack, the walk keys, a pad button
                 and the camera mode, and leaves everything else alone --
                 the moved ones move, the rest stay at their defaults, an
                 out-of-range value reads as its default.
     alias_old   only RunButtonKey / RunButtonPad in the file: read exactly
                 as before, and the new names answer the same.
     alias_both  both spellings present and disagreeing: KeyRun / PadRun win.
     alias_badnew the new spelling present but out of range: the valid old
                 spelling beside it wins, because "present" is not "parsed".
     save        the run save writes BOTH spellings and the camera save
                 writes CameraMode, and a key this program never heard of
                 survives the write untouched.
     padlayouts  the PadLayouts array: two objects parse and answer by
                 vid:pid, a field out of range reads as the generic
                 fallback's value, an object with no usable vid:pid is
                 dropped, a sign of 0 is refused, and an unknown pad is 0.
     padlayouts_save
                 a learned layout saved into a file that already has one
                 for another pad: both are in the array afterwards, a
                 re-save of the same vid:pid replaces rather than appends,
                 the launcher's own key survives, and the file re-reads in
                 a child to the same values.
     padlayouts_backslash
                 a name carrying a backslash pair: the fields after it and
                 the keys after the array still read, and a save drops the
                 backslash rather than escaping it.
     padlayouts_bad
                 a PadLayouts that is not an array, and one that is an array
                 with nothing usable in it, both read as no layouts and every
                 other key in the file still parses.
     padtranslate
                 hal/pad_backend.cpp's translation on a synthetic report
                 through a learned row (port_pad_selftest).

   find_settings looks beside the exe first, then SM64DS_ASSET_ROOT, then the
   working directory. The parent refuses to run if a settings.json is beside
   the exe -- a developer's own file would make every case below meaningless
   -- and clears SM64DS_ASSET_ROOT from the child's environment so the
   working directory is the file that is read. */

#include "hal/host_settings.h"
#include "hal/pad_backend.h"

#include <direct.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

extern "C" int host_setting_run_mode(void);
extern "C" int host_setting_run_key(void);
extern "C" int host_setting_run_pad(void);
extern "C" int host_setting_save_run(int mode, int key, int pad);

static int fails;

static void check(int cond, const char *what)
{
    if (cond) return;
    ++fails;
    fprintf(stderr, "  FAIL %s\n", what);
}

static void check_eq(int got, int want, const char *what)
{
    if (got == want) return;
    ++fails;
    fprintf(stderr, "  FAIL %s: got 0x%x want 0x%x\n", what, (unsigned)got,
            (unsigned)want);
}

static const int KEY_DFLT[HOST_KEY_COUNT] = {
    0x57, 0x53, 0x41, 0x44, 0x26, 0x28, 0x25, 0x27,
    0x20, 0x58, 0x11, 0x10, 0x0d, 0x08,
};
static const int PAD_DFLT[HOST_PAD_COUNT] = {
    0x1000, 0x2000, 0x20000, 0x4000, 0x0010, 0,   /* crouch = RT pseudo-mask */
};

static void check_defaults_except(const int *skip_key, int nskip_key,
                                  const int *skip_pad, int nskip_pad)
{
    for (int i = 0; i < HOST_KEY_COUNT; ++i) {
        int skip = 0;
        for (int k = 0; k < nskip_key; ++k) if (skip_key[k] == i) skip = 1;
        if (!skip) check_eq(host_setting_key(i), KEY_DFLT[i],
                            host_setting_key_name(i));
    }
    for (int i = 0; i < HOST_PAD_COUNT; ++i) {
        int skip = 0;
        for (int k = 0; k < nskip_pad; ++k) if (skip_pad[k] == i) skip = 1;
        if (!skip) check_eq(host_setting_pad(i), PAD_DFLT[i],
                            host_setting_pad_name(i));
    }
}

static char *slurp(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    static char buf[65536];
    size_t n = fread(buf, 1, sizeof buf - 1, f);
    fclose(f);
    buf[n] = '\0';
    return buf;
}

static int child(const char *which)
{
    if (!strcmp(which, "defaults")) {
        check_defaults_except(0, 0, 0, 0);
        check_eq(host_setting_run_key(), 0x10, "run key");
        check_eq(host_setting_run_pad(), 0x4000, "run pad");
        check_eq(host_setting_key(HOST_KEY_RUN), host_setting_run_key(),
                 "KeyRun agrees with RunButtonKey");
        check_eq(host_setting_pad(HOST_PAD_RUN), host_setting_run_pad(),
                 "PadRun agrees with RunButtonPad");
        check_eq(host_setting_camera_mode(), 0, "CameraMode analog");
        check_eq(host_setting_key(-1), 0, "out of range key is unbound");
        check_eq(host_setting_pad(99), 0, "out of range pad is unbound");
    } else if (!strcmp(which, "remap")) {
        const int sk[] = { HOST_KEY_JUMP, HOST_KEY_ATTACK, HOST_KEY_UP,
                           HOST_KEY_LEFT, HOST_KEY_DOWN, HOST_KEY_RIGHT,
                           HOST_KEY_UP_ALT, HOST_KEY_SELECT };
        const int sp[] = { HOST_PAD_JUMP, HOST_PAD_SELECT };
        check_defaults_except(sk, 8, sp, 2);
        check_eq(host_setting_key(HOST_KEY_JUMP), 0x4a, "KeyJump J");
        check_eq(host_setting_key(HOST_KEY_ATTACK), 0x4b, "KeyAttack K");
        check_eq(host_setting_key(HOST_KEY_UP), 0x49, "KeyUp I");
        check_eq(host_setting_key(HOST_KEY_LEFT), 0x4a, "KeyLeft J (two "
                 "actions on one key is legal)");
        check_eq(host_setting_key(HOST_KEY_DOWN), 0x4b, "KeyDown K");
        check_eq(host_setting_key(HOST_KEY_RIGHT), 0x4c, "KeyRight L");
        check_eq(host_setting_key(HOST_KEY_UP_ALT), 0, "KeyUpAlt unbound");
        /* 0x1234 is outside 0..0xff: a typo, so the default */
        check_eq(host_setting_key(HOST_KEY_SELECT), 0x08,
                 "KeySelect out of range reads as default");
        check_eq(host_setting_pad(HOST_PAD_JUMP), 0x8000, "PadJump Y");
        check_eq(host_setting_pad(HOST_PAD_SELECT), 0x0020, "PadSelect BACK");
        check_eq(host_setting_camera_mode(), 2, "CameraMode ds");
        /* the run pair was not in the file, so both spellings answer the
           default */
        check_eq(host_setting_run_key(), 0x10, "run key untouched");
    } else if (!strcmp(which, "camera_number")) {
        check_eq(host_setting_camera_mode(), 1, "CameraMode 1 is freecam");
    } else if (!strcmp(which, "alias_old")) {
        check_eq(host_setting_run_key(), 0x52, "RunButtonKey R");
        check_eq(host_setting_run_pad(), 0x8000, "RunButtonPad Y");
        check_eq(host_setting_key(HOST_KEY_RUN), 0x52, "KeyRun follows old");
        check_eq(host_setting_pad(HOST_PAD_RUN), 0x8000, "PadRun follows old");
    } else if (!strcmp(which, "alias_both")) {
        check_eq(host_setting_run_key(), 0x51, "KeyRun Q wins");
        check_eq(host_setting_run_pad(), 0x0200, "PadRun RB wins");
        check_eq(host_setting_key(HOST_KEY_RUN), 0x51, "KeyRun Q");
        check_eq(host_setting_pad(HOST_PAD_RUN), 0x0200, "PadRun RB");
    } else if (!strcmp(which, "alias_badnew")) {
        /* the new spelling is there but out of range: a typo, so the valid
           old spelling beside it must still win */
        check_eq(host_setting_run_key(), 0x52, "RunButtonKey R beats KeyRun 999");
        check_eq(host_setting_run_pad(), 0x8000, "RunButtonPad Y beats PadRun -1");
        check_eq(host_setting_key(HOST_KEY_RUN), 0x52, "KeyRun follows old");
        check_eq(host_setting_pad(HOST_PAD_RUN), 0x8000, "PadRun follows old");
    } else if (!strcmp(which, "save")) {
        check(host_setting_save_run(1, 0x51, 0x0200), "save_run wrote");
        check(host_setting_save_camera_mode(1), "save_camera_mode wrote");
        check_eq(host_setting_run_key(), 0x51, "live run key moved");
        check_eq(host_setting_key(HOST_KEY_RUN), 0x51, "live KeyRun moved");
        check_eq(host_setting_camera_mode(), 1, "live camera moved");
        const char *t = slurp("settings.json");
        check(t != 0, "settings.json exists after save");
        if (t) {
            check(strstr(t, "\"RunButtonKey\": 81") != 0, "RunButtonKey 81");
            check(strstr(t, "\"KeyRun\": 81") != 0, "KeyRun 81");
            check(strstr(t, "\"RunButtonPad\": 512") != 0, "RunButtonPad 512");
            check(strstr(t, "\"PadRun\": 512") != 0, "PadRun 512");
            check(strstr(t, "\"RunMode\": \"analog\"") != 0, "RunMode analog");
            check(strstr(t, "\"CameraMode\": \"freecam\"") != 0,
                  "CameraMode freecam");
            check(strstr(t, "\"Volume\": 37") != 0,
                  "the launcher's own key survived the write");
        }
    } else if (!strcmp(which, "padlayouts")) {
        HostPadLayout L, D;
        host_pad_layout_default(&D);
        check_eq(host_setting_pad_layout_count(), 2, "two layouts parsed");
        check(host_setting_pad_layout(1118, 654, &L), "1118:654 found");
        check_eq(L.a, 2, "a"); check_eq(L.b, 1, "b"); check_eq(L.x, 3, "x");
        check_eq(L.y, 0, "y"); check_eq(L.lb, 4, "lb"); check_eq(L.rb, 5, "rb");
        check_eq(L.back, 8, "back"); check_eq(L.start, 9, "start");
        check_eq(L.lt_btn, 6, "lt_btn"); check_eq(L.rt_btn, -1, "rt_btn -1");
        check_eq(L.rt_axis, 3, "rt_axis"); check_eq(L.lx_axis, 2, "lx_axis");
        check_eq(L.lx_sign, -1, "lx_sign"); check_eq(L.ly_axis, 5, "ly_axis");
        check_eq(L.ly_sign, 1, "ly_sign"); check_eq(L.rx_axis, 0, "rx_axis");
        check(!strcmp(L.name, "USB Gamepad"), "name");
        /* the second object: y is 99 (out of range), lthumb is "abc",
           rx_sign is 0 -- each of those reads as the fallback; the rest of
           the object is honoured */
        check(host_setting_pad_layout(0x1234, 0x5678, &L), "4660:22136 found");
        check_eq(L.a, 7, "second a");
        check_eq(L.y, D.y, "y out of range reads as fallback");
        check_eq(L.lthumb, D.lthumb, "lthumb not a number reads as fallback");
        check_eq(L.rx_sign, D.rx_sign, "rx_sign 0 reads as fallback");
        check_eq(L.ly_sign, -1, "second ly_sign kept");
        check(!host_setting_pad_layout(1, 2, &L), "unknown pad is 0");
        /* the object with vid 0 and the one with no pid were dropped */
        check(!host_setting_pad_layout(0, 5, &L), "vid 0 dropped");
        check(!host_setting_pad_layout(77, 0, &L), "no pid dropped");
        check_eq(D.a, 0, "fallback a"); check_eq(D.ly_sign, -1, "fallback ly_sign");
    } else if (!strcmp(which, "padlayouts_save")) {
        HostPadLayout L;
        host_pad_layout_default(&L);
        L.vid = 0x2345; L.pid = 0x6789;
        L.a = 3; L.b = 2; L.x = 1; L.y = 0; L.lx_axis = 3; L.lx_sign = -1;
        L.rt_btn = -1; L.rt_axis = 4;
        snprintf(L.name, sizeof L.name, "Say \"hi\" pad");
        check(host_setting_save_pad_layout(&L), "save wrote");
        check_eq(host_setting_pad_layout_count(), 2, "two after the save");
        /* the same pad again with one change: replaces, does not append */
        L.b = 9;
        check(host_setting_save_pad_layout(&L), "re-save wrote");
        check_eq(host_setting_pad_layout_count(), 2, "still two after re-save");
        /* out of range on the way in is clamped to the fallback before the
           write, so the file never carries a value a reader would refuse */
        L.vid = 0x1111; L.pid = 0x2222; L.y = 200; L.ly_sign = 0;
        check(host_setting_save_pad_layout(&L), "third save wrote");
        check_eq(host_setting_pad_layout_count(), 3, "three");
        check(host_setting_pad_layout(0x1111, 0x2222, &L), "third found");
        check_eq(L.y, 3, "y 200 clamped to fallback");
        check_eq(L.ly_sign, -1, "ly_sign 0 clamped to fallback");
        HostPadLayout bad;
        host_pad_layout_default(&bad);
        check(!host_setting_save_pad_layout(&bad), "vid 0 refused");
        const char *t = slurp("settings.json");
        check(t != 0, "settings.json exists after save");
        if (t) {
            check(strstr(t, "\"Volume\": 37") != 0, "the launcher's key survived");
            check(strstr(t, "\"vid\": 1118") != 0, "the earlier pad survived");
            check(strstr(t, "\"vid\": 9029") != 0, "the saved pad is there");
            check(strstr(t, "\"b\": 9") != 0, "the re-save's value is there");
            check(strstr(t, "\"b\": 2,") == 0, "the replaced value is gone");
            check(strstr(t, "Say \\\"hi\\\" pad") != 0, "the name is escaped");
        }
    } else if (!strcmp(which, "padlayouts_backslash")) {
        /* a name carrying a backslash pair, then more fields in the same
           object and a key after the array: all of it must still read */
        HostPadLayout L;
        check_eq(host_setting_pad_layout_count(), 1, "one layout");
        check(host_setting_pad_layout(1118, 654, &L), "found past the name");
        check(!strcmp(L.name, "Pad \\ Co"), "name with the backslash");
        check_eq(L.b, 5, "field after the name read");
        check_eq(host_setting_key(HOST_KEY_JUMP), 0x4a, "key after the array read");
        check_eq(host_setting_camera_mode(), 2, "second key after the array read");
        /* and a save of a name with a backslash writes it without one */
        snprintf(L.name, sizeof L.name, "Back\\slash \"q\"");
        check(host_setting_save_pad_layout(&L), "save wrote");
        const char *t = slurp("settings.json");
        check(t != 0, "file exists");
        if (t) {
            check(strstr(t, "Backslash \\\"q\\\"") != 0, "backslash dropped, quote escaped");
            check(strstr(t, "\"KeyJump\": 74") != 0, "KeyJump survived");
        }
    } else if (!strcmp(which, "padlayouts_reread")) {
        /* the file the save case left behind, read fresh by this process */
        HostPadLayout L;
        check_eq(host_setting_pad_layout_count(), 3, "three re-read");
        check(host_setting_pad_layout(0x2345, 0x6789, &L), "saved pad re-read");
        check_eq(L.b, 9, "re-read b"); check_eq(L.lx_axis, 3, "re-read lx_axis");
        check_eq(L.lx_sign, -1, "re-read lx_sign"); check_eq(L.rt_axis, 4, "re-read rt_axis");
        check_eq(L.rt_btn, -1, "re-read rt_btn");
        check(!strcmp(L.name, "Say \"hi\" pad"), "re-read name unescaped");
        check_eq(host_setting_key(HOST_KEY_JUMP), 0x20, "the other keys still default");
    } else if (!strcmp(which, "padlayouts_bad")) {
        check_eq(host_setting_pad_layout_count(), 0, "unusable PadLayouts is none");
        check_eq(host_setting_key(HOST_KEY_JUMP), 0x4a, "KeyJump J still parsed");
        check_eq(host_setting_camera_mode(), 2, "CameraMode ds still parsed");
    } else if (!strcmp(which, "padtranslate")) {
        check(port_pad_selftest(), "pad_backend translation selftest");
    } else {
        fprintf(stderr, "  unknown case %s\n", which);
        return 2;
    }
    return fails ? 1 : 0;
}

static int write_file(const char *path, const char *text)
{
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    fputs(text, f);
    fclose(f);
    return 1;
}

/* json is the file to write, 0 for no file at all, or (const char *)1 for
   "the file the previous case left" -- which only exists when that case ran
   with keep_after set, the shape the save-then-reread pair needs. */
static int run_case_x(const char *exe, const char *dir, const char *which,
                      const char *json, int keep_after)
{
    char path[MAX_PATH];
    snprintf(path, sizeof path, "%s\\settings.json", dir);
    const int reuse = json == (const char *)1;
    if (!reuse) remove(path);
    if (json && !reuse && !write_file(path, json)) {
        fprintf(stderr, "smoke_settings: cannot write %s\n", path);
        return 1;
    }
    char cwd[MAX_PATH];
    if (!_getcwd(cwd, sizeof cwd)) return 1;
    if (_chdir(dir) != 0) return 1;
    const intptr_t rc = _spawnl(_P_WAIT, exe, "smoke_settings", which, (char *)0);
    _chdir(cwd);
    if (!keep_after) remove(path);
    if (rc != 0) {
        fprintf(stderr, "smoke_settings: case %s FAILED (rc %d)\n", which,
                (int)rc);
        return 1;
    }
    printf("smoke_settings: %s ok\n", which);
    return 0;
}

static int run_case(const char *exe, const char *dir, const char *which,
                    const char *json)
{
    return run_case_x(exe, dir, which, json, 0);
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
        snprintf(beside + strlen(beside), sizeof beside - strlen(beside),
                 "\\settings.json");
        FILE *f = fopen(beside, "rb");
        if (f) {
            fclose(f);
            fprintf(stderr, "smoke_settings: %s exists; the loader reads "
                            "beside the exe first, so every case here would "
                            "read it. Move it away and rerun.\n", beside);
            return 1;
        }
    }
    /* the working directory must be the file that is read */
    _putenv("SM64DS_ASSET_ROOT=");
    _putenv("SM64DS_INSTANCE=");

    char tmp[MAX_PATH], dir[MAX_PATH];
    if (!GetTempPathA(MAX_PATH, tmp)) return 1;
    snprintf(dir, sizeof dir, "%ssm64ds_smoke_settings_%lu", tmp,
             (unsigned long)GetCurrentProcessId());
    _mkdir(dir);

    int bad = 0;
    bad |= run_case(exe, dir, "defaults", 0);
    bad |= run_case(exe, dir, "remap",
        "{\n"
        "  \"KeyJump\": 74,\n"
        "  \"KeyAttack\": \"75\",\n"
        "  \"KeyUp\": 73,\n"
        "  \"KeyLeft\": 74,\n"
        "  \"KeyDown\": 75,\n"
        "  \"KeyRight\": 76,\n"
        "  \"KeyUpAlt\": 0,\n"
        "  \"KeySelect\": 4660,\n"
        "  \"PadJump\": 32768,\n"
        "  \"PadSelect\": 32,\n"
        "  \"CameraMode\": \"ds\",\n"
        "  \"Volume\": 50\n"
        "}\n");
    bad |= run_case(exe, dir, "camera_number", "{ \"CameraMode\": 1 }");
    bad |= run_case(exe, dir, "alias_old",
        "{ \"RunButtonKey\": 82, \"RunButtonPad\": 32768 }");
    bad |= run_case(exe, dir, "alias_both",
        "{ \"RunButtonKey\": 82, \"KeyRun\": 81,\n"
        "  \"RunButtonPad\": 32768, \"PadRun\": 512 }");
    bad |= run_case(exe, dir, "alias_badnew",
        "{ \"RunButtonKey\": 82, \"KeyRun\": 999,\n"
        "  \"RunButtonPad\": 32768, \"PadRun\": -1 }");
    bad |= run_case(exe, dir, "save", "{\n  \"Volume\": 37\n}\n");
    bad |= run_case(exe, dir, "padlayouts",
        "{\n"
        "  \"Volume\": 37,\n"
        "  \"PadLayouts\": [\n"
        "    { \"vid\": 1118, \"pid\": 654, \"name\": \"USB Gamepad\",\n"
        "      \"a\": 2, \"b\": 1, \"x\": 3, \"y\": 0, \"lb\": 4, \"rb\": 5,\n"
        "      \"back\": 8, \"start\": 9, \"lt_btn\": 6, \"rt_btn\": -1,\n"
        "      \"rt_axis\": 3, \"lx_axis\": 2, \"lx_sign\": -1,\n"
        "      \"ly_axis\": 5, \"ly_sign\": 1, \"rx_axis\": 0 },\n"
        "    { \"vid\": 0, \"pid\": 5, \"a\": 1 },\n"
        "    { \"vid\": 77, \"a\": 1 },\n"
        "    { \"vid\": 4660, \"pid\": 22136, \"a\": 7, \"y\": 99,\n"
        "      \"lthumb\": \"abc\", \"rx_sign\": 0, \"ly_sign\": -1 }\n"
        "  ],\n"
        "  \"KeyJump\": 74\n"
        "}\n");
    /* the save case writes into a file that already carries one pad, and
       leaves the file for the re-read case, which is a fresh process */
    bad |= run_case_x(exe, dir, "padlayouts_save",
        "{\n"
        "  \"Volume\": 37,\n"
        "  \"PadLayouts\": [ { \"vid\": 1118, \"pid\": 654, \"a\": 2 } ]\n"
        "}\n", 1);
    bad |= run_case(exe, dir, "padlayouts_reread", (const char *)1);
    bad |= run_case(exe, dir, "padlayouts_backslash",
        "{\n"
        "  \"PadLayouts\": [ { \"vid\": 1118, \"pid\": 654,\n"
        "      \"name\": \"Pad \\\\ Co\", \"b\": 5 } ],\n"
        "  \"KeyJump\": 74,\n"
        "  \"CameraMode\": \"ds\"\n"
        "}\n");
    /* not an array, then an array with nothing usable in it */
    bad |= run_case(exe, dir, "padlayouts_bad",
        "{ \"KeyJump\": 74, \"PadLayouts\": \"nope\", \"CameraMode\": \"ds\" }");
    bad |= run_case(exe, dir, "padlayouts_bad",
        "{\n"
        "  \"KeyJump\": 74,\n"
        "  \"PadLayouts\": [ 5, \"x\", { \"vid\": \"abc\", \"pid\": 1 }, [ ] ],\n"
        "  \"CameraMode\": \"ds\"\n"
        "}\n");
    bad |= run_case(exe, dir, "padtranslate", 0);
    _rmdir(dir);
    if (bad) {
        printf("smoke_settings: FAIL\n");
        return 1;
    }
    printf("smoke_settings: ok, 15 cases\n");
    return 0;
}
