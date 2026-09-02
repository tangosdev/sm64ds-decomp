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

   find_settings looks beside the exe first, then SM64DS_ASSET_ROOT, then the
   working directory. The parent refuses to run if a settings.json is beside
   the exe -- a developer's own file would make every case below meaningless
   -- and clears SM64DS_ASSET_ROOT from the child's environment so the
   working directory is the file that is read. */

#include "hal/host_settings.h"

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

static int run_case(const char *exe, const char *dir, const char *which,
                    const char *json)
{
    char path[MAX_PATH];
    snprintf(path, sizeof path, "%s\\settings.json", dir);
    remove(path);
    if (json && !write_file(path, json)) {
        fprintf(stderr, "smoke_settings: cannot write %s\n", path);
        return 1;
    }
    char cwd[MAX_PATH];
    if (!_getcwd(cwd, sizeof cwd)) return 1;
    if (_chdir(dir) != 0) return 1;
    const intptr_t rc = _spawnl(_P_WAIT, exe, "smoke_settings", which, (char *)0);
    _chdir(cwd);
    remove(path);
    if (rc != 0) {
        fprintf(stderr, "smoke_settings: case %s FAILED (rc %d)\n", which,
                (int)rc);
        return 1;
    }
    printf("smoke_settings: %s ok\n", which);
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
    _rmdir(dir);
    if (bad) {
        printf("smoke_settings: FAIL\n");
        return 1;
    }
    printf("smoke_settings: ok, 7 cases\n");
    return 0;
}
