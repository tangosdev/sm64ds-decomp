#!/usr/bin/env python3
"""Steam Deck Setup: get the SM64DS PC port into Steam in one go.

Started by double-clicking "Steam Deck Setup" in the kit folder (Desktop Mode),
or by hand:   python3 steam_deck_setup.py

What it does, in order:
  1. checks that a .nds dump is in the PLACE YOUR ROM HERE folder;
  2. unpacks the game data with extract_assets.py, unless that was done before
     (the same check launch.sh makes);
  3. adds a non-Steam game called "Super Mario 64 DS PC" that starts
     SM64DSLauncher.exe from this folder, for every Steam user on this machine,
     and sets it to run with Proton Experimental. Steam has to be closed while
     its files are written, so it asks first, closes Steam, writes, and starts
     Steam again. Both Steam files are backed up first. Running it again
     updates the same entry instead of adding a second one;
  4. says where to find the game.

If anything goes wrong it prints the manual steps instead.

Options (mostly for testing):
  --dry-run          say what would happen, write nothing, close nothing
  --steam-root DIR   use this Steam folder instead of looking for one (repeatable)
  --kit DIR          the kit folder (default: the folder this script is in)
  --yes              do not ask; answer yes (closes Steam without asking)
  --no-gui           questions and messages in the terminal only
  --no-pause         do not wait for Enter before closing the window

Only the Python 3 standard library is used (SteamOS has python3 and no pip).

The non-Steam game id is the one Steam ROM Manager and BoilR use:
    appid = crc32(Exe + AppName) | 0x80000000
where Exe is the Exe field exactly as stored in shortcuts.vdf (with its double
quotes). It is written into the entry's appid field, and the same number (as an
unsigned decimal) keys the entry's CompatToolMapping in config/config.vdf.
"""
import argparse
import os
import shutil
import struct
import subprocess
import sys
import time
import zlib

APP_NAME = "Super Mario 64 DS PC"
LAUNCHER = "SM64DSLauncher.exe"
COMPAT_TOOL = "proton_experimental"
COMPAT_TOOL_DIR = "Proton - Experimental"
ROM_DIRS = ("PLACE YOUR ROM HERE", "PLACE EU ROM HERE")
# The files launch.sh checks for; if any is missing the data is unpacked again.
EXTRACTED_FILES = (
    "build/assets/files.tsv", "build/assets/handles.tsv", "build/assets/nitrofs.tsv",
    "build/assets/nitrofs_fnt.bin", "build/assets/nitrofs_fat.bin",
    "build/assets/nitrofs_ovt9.bin", "build/assets/romdata.bin",
    "build/assets/romdata.manifest", "extracted/dsd/files/data/sound_data.sdat",
)
FLATPAK_ID = "com.valvesoftware.Steam"

MANUAL_STEPS = """\
You can still do it by hand. In Desktop Mode:

1. Put your .nds file in the folder named PLACE YOUR ROM HERE.
2. Open a terminal in the kit folder (in Dolphin: right-click inside the
   folder, Open Terminal Here) and run:   python3 extract_assets.py
   It unpacks the game data once (about a minute).
3. In Steam choose Games, Add a Non-Steam Game to My Library, Browse, pick
   SM64DSLauncher.exe in the kit folder, and add it.
4. Right-click the new entry, Properties, Compatibility, tick "Force the use
   of a specific Steam Play compatibility tool" and pick Proton Experimental.
   (No Proton Experimental in the list? In the Library, set the filter to
   Tools, install Proton Experimental, then pick it.)
5. Switch to Gaming Mode and start it from your library.

README.txt in the kit folder has the same steps."""


class SetupError(Exception):
    """Something went wrong; the message is for the player."""


# --------------------------------------------------------------------------
# Talking to the player

class UI:
    def __init__(self, gui, assume_yes):
        self.assume_yes = assume_yes
        self.tool = None
        if gui and (os.environ.get("DISPLAY") or os.environ.get("WAYLAND_DISPLAY")):
            for t in ("kdialog", "zenity"):
                if shutil.which(t):
                    self.tool = t
                    break

    def say(self, text=""):
        print(text, flush=True)

    def _run(self, args):
        try:
            return subprocess.call(args, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        except OSError:
            return None

    def info(self, text, error=False):
        """Print a message, and also show it in a box when there is a desktop."""
        self.say()
        self.say(text)
        self.say()
        if self.tool == "kdialog":
            self._run(["kdialog", "--title", "Steam Deck Setup",
                       "--error" if error else "--msgbox", text])
        elif self.tool == "zenity":
            self._run(["zenity", "--error" if error else "--info", "--no-wrap",
                       "--title", "Steam Deck Setup", "--text", text])

    def ask(self, text):
        """Yes or no. Defaults to no when nobody can answer."""
        self.say()
        self.say(text)
        if self.assume_yes:
            self.say("(--yes given: yes)")
            return True
        rc = None
        if self.tool == "kdialog":
            rc = self._run(["kdialog", "--title", "Steam Deck Setup", "--yesno", text])
        elif self.tool == "zenity":
            rc = self._run(["zenity", "--question", "--no-wrap",
                            "--title", "Steam Deck Setup", "--text", text])
        if rc is not None and rc in (0, 1):
            self.say("Answer: " + ("yes" if rc == 0 else "no"))
            return rc == 0
        if sys.stdin is None or not sys.stdin.isatty():
            self.say("(nobody to answer: no)")
            return False
        while True:
            try:
                a = input("Type y for yes or n for no, then Enter: ").strip().lower()
            except EOFError:
                return False
            if a in ("y", "yes"):
                return True
            if a in ("n", "no"):
                return False


# --------------------------------------------------------------------------
# Binary VDF (userdata/<id>/config/shortcuts.vdf)
#
# A map is a list of [key, type, value] so the order and every field Steam
# wrote survive a rewrite untouched. Types: 0 map, 1 string, 2 int32, 3 float32,
# 4 pointer, 6 colour, 7 uint64, 0x0A int64, 8 end of map.

T_MAP, T_STR, T_INT, T_FLOAT, T_PTR, T_COLOR, T_U64, T_END, T_I64 = 0, 1, 2, 3, 4, 6, 7, 8, 0x0A
_FIXED = {T_INT: 4, T_FLOAT: 4, T_PTR: 4, T_COLOR: 4, T_U64: 8, T_I64: 8}


def _cstr(data, i):
    j = data.index(b"\0", i)
    return data[i:j].decode("utf-8", "surrogateescape"), j + 1


def bvdf_read_map(data, i):
    out = []
    while True:
        if i >= len(data):
            raise ValueError("file ends inside a map")
        t = data[i]
        i += 1
        if t == T_END:
            return out, i
        key, i = _cstr(data, i)
        if t == T_MAP:
            val, i = bvdf_read_map(data, i)
        elif t == T_STR:
            val, i = _cstr(data, i)
        elif t in _FIXED:
            n = _FIXED[t]
            if i + n > len(data):
                raise ValueError("file ends inside a value")
            val = data[i:i + n]          # raw bytes: written back exactly as read
            i += n
        else:
            raise ValueError("unknown value type 0x%02x at byte %d" % (t, i - 1))
        out.append([key, t, val])


def bvdf_loads(data):
    root, i = bvdf_read_map(data, 0)
    if i != len(data):
        raise ValueError("%d stray bytes after the end" % (len(data) - i))
    return root


def bvdf_write_map(m, out):
    for key, t, val in m:
        out.append(bytes([t]))
        out.append(key.encode("utf-8", "surrogateescape") + b"\0")
        if t == T_MAP:
            bvdf_write_map(val, out)
        elif t == T_STR:
            out.append(val.encode("utf-8", "surrogateescape") + b"\0")
        else:
            out.append(val)
    out.append(bytes([T_END]))


def bvdf_dumps(root):
    out = []
    bvdf_write_map(root, out)
    return b"".join(out)


def bvdf_get(m, key):
    for e in m:
        if e[0].lower() == key.lower():
            return e
    return None


def bvdf_set(m, key, t, val):
    e = bvdf_get(m, key)
    if e is None:
        m.append([key, t, val])
    else:
        e[1], e[2] = t, val


def u32_bytes(v):
    return struct.pack("<I", v & 0xFFFFFFFF)


def bytes_u32(b):
    return struct.unpack("<I", b)[0]


def shortcut_appid(exe_field, app_name):
    """Steam ROM Manager / BoilR: crc32(Exe + AppName) with the top bit set."""
    return (zlib.crc32((exe_field + app_name).encode("utf-8")) & 0xFFFFFFFF) | 0x80000000


def _unquote(s):
    s = s.strip()
    if len(s) >= 2 and s[0] == '"' and s[-1] == '"':
        s = s[1:-1]
    return s


def _same_file(a, b):
    try:
        return os.path.realpath(a) == os.path.realpath(b)
    except (OSError, ValueError):
        return False


def upsert_shortcut(root, exe_path, start_dir):
    """Add our entry to a parsed shortcuts.vdf, or update the one already there.

    Returns (appid, action, duplicates): action is "added" or "updated";
    duplicates counts further entries that also match (left alone)."""
    sc = bvdf_get(root, "shortcuts")
    if sc is None:
        sc = ["shortcuts", T_MAP, []]
        root.append(sc)
    if sc[1] != T_MAP:
        raise ValueError("'shortcuts' is not a map")
    entries = sc[2]

    exe_field = '"%s"' % exe_path
    start_field = '"%s/"' % start_dir.rstrip("/")
    matches = []
    for e in entries:
        if e[1] != T_MAP:
            continue
        name = bvdf_get(e[2], "AppName")
        exe = bvdf_get(e[2], "Exe")
        if name is not None and name[1] == T_STR and name[2].strip().lower() == APP_NAME.lower():
            matches.append(e)
        elif exe is not None and exe[1] == T_STR and _same_file(_unquote(exe[2]), exe_path):
            matches.append(e)

    if matches:
        m = matches[0][2]
        bvdf_set(m, "AppName", T_STR, APP_NAME)
        bvdf_set(m, "Exe", T_STR, exe_field)
        bvdf_set(m, "StartDir", T_STR, start_field)
        cur = bvdf_get(m, "appid")
        if cur is not None and cur[1] == T_INT and bytes_u32(cur[2]) != 0:
            appid = bytes_u32(cur[2])      # keep the id Steam already knows it by
        else:
            appid = shortcut_appid(exe_field, APP_NAME)
            bvdf_set(m, "appid", T_INT, u32_bytes(appid))
        return appid, "updated", len(matches) - 1

    appid = shortcut_appid(exe_field, APP_NAME)
    used = [int(e[0]) for e in entries if e[0].isdigit()]
    key = str(max(used) + 1 if used else 0)
    entry = [
        ["appid", T_INT, u32_bytes(appid)],
        ["AppName", T_STR, APP_NAME],
        ["Exe", T_STR, exe_field],
        ["StartDir", T_STR, start_field],
        ["icon", T_STR, ""],
        ["ShortcutPath", T_STR, ""],
        ["LaunchOptions", T_STR, ""],
        ["IsHidden", T_INT, u32_bytes(0)],
        ["AllowDesktopConfig", T_INT, u32_bytes(1)],
        ["AllowOverlay", T_INT, u32_bytes(1)],
        ["OpenVR", T_INT, u32_bytes(0)],
        ["Devkit", T_INT, u32_bytes(0)],
        ["DevkitGameID", T_STR, ""],
        ["DevkitOverrideAppID", T_INT, u32_bytes(0)],
        ["LastPlayTime", T_INT, u32_bytes(0)],
        ["FlatpakAppID", T_STR, ""],
        ["tags", T_MAP, []],
    ]
    entries.append([key, T_MAP, entry])
    return appid, "added", 0


# --------------------------------------------------------------------------
# Text VDF (config/config.vdf, steamapps/libraryfolders.vdf)
#
# Same shape as above: a map is a list of [key, value], value a str or a map.

def tvdf_loads(text):
    toks = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c in " \t\r\n\ufeff":
            i += 1
        elif c == "/" and text.startswith("//", i):
            j = text.find("\n", i)
            i = n if j < 0 else j + 1
        elif c in "{}":
            toks.append(c)
            i += 1
        elif c == "[":                      # conditional like [$WIN32]: ignored
            j = text.find("]", i)
            i = n if j < 0 else j + 1
        elif c == '"':
            i += 1
            buf = []
            while True:
                if i >= n:
                    raise ValueError("a quoted string never ends")
                c = text[i]
                if c == "\\" and i + 1 < n:
                    nx = text[i + 1]
                    buf.append({"n": "\n", "t": "\t", "\\": "\\", '"': '"'}.get(nx, "\\" + nx))
                    i += 2
                elif c == '"':
                    i += 1
                    break
                else:
                    buf.append(c)
                    i += 1
            toks.append(("s", "".join(buf)))
        else:
            j = i
            while j < n and text[j] not in ' \t\r\n{}"':
                j += 1
            toks.append(("s", text[i:j]))
            i = j

    pos = [0]

    def parse_map(top):
        out = []
        while pos[0] < len(toks):
            t = toks[pos[0]]
            if t == "}":
                if top:
                    raise ValueError("a '}' with no '{'")
                pos[0] += 1
                return out
            if t == "{":
                raise ValueError("a '{' where a key should be")
            key = t[1]
            pos[0] += 1
            if pos[0] >= len(toks):
                raise ValueError("key '%s' has no value" % key)
            v = toks[pos[0]]
            if v == "{":
                pos[0] += 1
                out.append([key, parse_map(False)])
            elif v == "}":
                raise ValueError("key '%s' has no value" % key)
            else:
                pos[0] += 1
                out.append([key, v[1]])
        if not top:
            raise ValueError("a '{' is never closed")
        return out

    return parse_map(True)


def _tq(s):
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'


def tvdf_dumps(m, depth=0):
    tab = "\t" * depth
    lines = []
    for k, v in m:
        if isinstance(v, list):
            lines.append(tab + _tq(k))
            lines.append(tab + "{")
            if v:
                lines.append(tvdf_dumps(v, depth + 1).rstrip("\n"))
            lines.append(tab + "}")
        else:
            lines.append(tab + _tq(k) + "\t\t" + _tq(v))
    return "\n".join(lines) + "\n"


def tvdf_get(m, key):
    for e in m:
        if e[0].lower() == key.lower():
            return e
    return None


def tvdf_child(m, key):
    """The map under key, made if missing. Matches keys the way Steam does
    (case does not matter: config.vdf has both "Valve" and "valve" in the wild)."""
    e = tvdf_get(m, key)
    if e is None:
        e = [key, []]
        m.append(e)
    elif not isinstance(e[1], list):
        raise ValueError("'%s' is a value, not a section" % key)
    return e[1]


def set_compat_tool(cfg, appid):
    """Point one app id at Proton Experimental in a parsed config.vdf."""
    m = tvdf_child(cfg, "InstallConfigStore")
    for k in ("Software", "Valve", "Steam", "CompatToolMapping"):
        m = tvdf_child(m, k)
    want = [["name", COMPAT_TOOL], ["config", ""], ["priority", "250"]]
    e = tvdf_get(m, str(appid))
    if e is not None and isinstance(e[1], list):
        before = [list(x) for x in e[1]]
        for k, v in want:
            f = tvdf_get(e[1], k)
            if f is None:
                e[1].append([k, v])
            else:
                f[1] = v
        return "unchanged" if e[1] == before else "updated"
    if e is not None:
        e[1] = want
        return "updated"
    m.append([str(appid), want])
    return "added"


def read_compat_tool(cfg, appid):
    m = cfg
    for k in ("InstallConfigStore", "Software", "Valve", "Steam", "CompatToolMapping", str(appid)):
        e = tvdf_get(m, k)
        if e is None or not isinstance(e[1], list):
            return None
        m = e[1]
    f = tvdf_get(m, "name")
    return f[1] if f else None


# --------------------------------------------------------------------------
# Steam on disk

def candidate_steam_roots():
    home = os.path.expanduser("~")
    return [
        os.path.join(home, ".local/share/Steam"),
        os.path.join(home, ".steam/steam"),
        os.path.join(home, ".steam/root"),
        os.path.join(home, ".var/app", FLATPAK_ID, ".local/share/Steam"),
        os.path.join(home, ".var/app", FLATPAK_ID, "data/Steam"),
    ]


def find_steam_roots(given):
    seen, roots = set(), []
    for r in (given or candidate_steam_roots()):
        if not os.path.isdir(os.path.join(r, "userdata")) and not given:
            continue
        real = os.path.realpath(r)
        if real in seen:
            continue
        seen.add(real)
        roots.append(r)
    return roots


def steam_users(root):
    ud = os.path.join(root, "userdata")
    if not os.path.isdir(ud):
        return []
    # "0" is not an account (Steam keeps shared settings there before anyone signs in).
    return sorted(d for d in os.listdir(ud)
                  if d.isdigit() and d != "0" and os.path.isdir(os.path.join(ud, d)))


def library_folders(root):
    out = [root]
    lf = os.path.join(root, "steamapps", "libraryfolders.vdf")
    try:
        with open(lf, "r", encoding="utf-8", errors="replace") as f:
            data = tvdf_loads(f.read())
        top = tvdf_get(data, "libraryfolders") or tvdf_get(data, "LibraryFolders")
        for k, v in (top[1] if top and isinstance(top[1], list) else []):
            if isinstance(v, list):
                p = tvdf_get(v, "path")
                if p and isinstance(p[1], str):
                    out.append(p[1])
            elif k.isdigit():
                out.append(v)                 # the old flat form: "1" "/path"
    except (OSError, ValueError):
        pass
    return out


def proton_experimental_installed(root):
    for lib in library_folders(root):
        if os.path.isfile(os.path.join(lib, "steamapps", "common", COMPAT_TOOL_DIR, "proton")):
            return True
    return False


# --------------------------------------------------------------------------
# The Steam process (Linux only; elsewhere it is never "running")

def steam_pids():
    if not sys.platform.startswith("linux") or not os.path.isdir("/proc"):
        return []
    pids = []
    for d in os.listdir("/proc"):
        if not d.isdigit():
            continue
        try:
            with open("/proc/%s/stat" % d) as f:
                stat = f.read()
        except OSError:
            continue
        # /proc/<pid>/stat is "pid (comm) state ...". "steam" is the client itself;
        # steamwebhelper and friends exit with it. A zombie (state Z) is already gone:
        # the "steam -shutdown" helper this script starts ends up as one for a moment.
        lp, rp = stat.find("("), stat.rfind(")")
        if lp < 0 or rp < 0:
            continue
        comm, state = stat[lp + 1:rp], stat[rp + 2:rp + 3]
        if comm == "steam" and state != "Z":
            pids.append(int(d))
    return pids


def steam_command(root, *args):
    if FLATPAK_ID in root and shutil.which("flatpak"):
        return ["flatpak", "run", FLATPAK_ID] + list(args)
    exe = shutil.which("steam")
    if exe:
        return [exe] + list(args)
    sh = os.path.join(root, "steam.sh")
    if os.path.isfile(sh):
        return ["/bin/sh", sh] + list(args)
    return None


def shut_down_steam(ui, root, timeout=90):
    cmd = steam_command(root, "-shutdown")
    if not cmd:
        raise SetupError("Steam is running, but the steam command was not found to close it. "
                         "Close Steam yourself (Steam menu, Exit) and run this again.")
    ui.say("Closing Steam: " + " ".join(cmd))
    try:
        helper = subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                                  stdin=subprocess.DEVNULL, start_new_session=True)
    except OSError as e:
        raise SetupError("Could not ask Steam to close: %s" % e)
    t0 = time.time()
    while time.time() - t0 < timeout:
        helper.poll()                # reap the helper so it does not count as Steam
        if not steam_pids():
            time.sleep(2)            # let it finish writing its own files
            if not steam_pids():
                ui.say("Steam is closed.")
                return
        time.sleep(1)
        sys.stdout.write(".")
        sys.stdout.flush()
    raise SetupError("Steam did not close within %d seconds. Close it yourself "
                     "(Steam menu, Exit) and run this again." % timeout)


def start_steam(ui, root):
    cmd = steam_command(root)
    if not cmd:
        ui.say("Start Steam yourself (it could not be started from here).")
        return
    ui.say("Starting Steam again.")
    try:
        subprocess.Popen(cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
                         stdin=subprocess.DEVNULL, start_new_session=True, close_fds=True)
    except OSError as e:
        ui.say("Steam could not be started from here (%s). Start it yourself." % e)


# --------------------------------------------------------------------------
# Files

def backup(path, stamp, dry):
    if not os.path.isfile(path):
        return None
    dst = "%s.bak-%s" % (path, stamp)
    if not dry:
        shutil.copy2(path, dst)
    return dst


def write_atomic(path, data):
    tmp = path + ".sm64ds-tmp"
    with open(tmp, "wb") as f:
        f.write(data)
        f.flush()
        os.fsync(f.fileno())
    os.replace(tmp, path)


# --------------------------------------------------------------------------
# The steps

def find_roms(kit):
    for name in ROM_DIRS:
        d = os.path.join(kit, name)
        if os.path.isdir(d):
            found = sorted(n for n in os.listdir(d) if n.lower().endswith(".nds"))
            if found:
                return [os.path.join(d, n) for n in found]
    return sorted(os.path.join(kit, n) for n in os.listdir(kit)
                  if n.lower().endswith(".nds") and os.path.isfile(os.path.join(kit, n)))


def step_rom(ui, kit):
    ui.say("Step 1 of 3: your cartridge dump")
    roms = find_roms(kit)
    if not roms:
        ui.info("No .nds file was found.\n\n"
                "Copy the .nds dump of your own Super Mario 64 DS cartridge (European) "
                "into the folder named\n\n    PLACE YOUR ROM HERE\n\n"
                "in this kit, then double-click Steam Deck Setup again.", error=True)
        return False
    if len(roms) > 1:
        ui.info("There is more than one .nds file in PLACE YOUR ROM HERE:\n\n    " +
                "\n    ".join(os.path.basename(r) for r in roms) +
                "\n\nLeave only the Super Mario 64 DS one there, then run this again.", error=True)
        return False
    ui.say("    found " + os.path.basename(roms[0]))
    return True


def extracted_already(kit):
    return all(os.path.isfile(os.path.join(kit, f)) for f in EXTRACTED_FILES)


def step_extract(ui, kit, dry):
    ui.say()
    ui.say("Step 2 of 3: unpacking the game data")
    if extracted_already(kit):
        ui.say("    already unpacked, skipping")
        return
    script = os.path.join(kit, "extract_assets.py")
    if not os.path.isfile(script):
        raise SetupError("extract_assets.py is missing from the kit folder. The kit is incomplete; "
                         "unzip it again.")
    cmd = [sys.executable or "python3", script, "--destination", kit]
    if dry:
        ui.say("    (dry run) would run: " + " ".join(cmd))
        return
    ui.say("    this takes about a minute, once")
    reason, tail = None, []
    p = subprocess.Popen(cmd, cwd=kit, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                         universal_newlines=True, bufsize=1)
    last = ""
    for line in p.stdout:
        line = line.rstrip("\n")
        if line.startswith("##PROGRESS "):
            parts = line.split(" ", 2)
            try:
                pct = int(parts[1])
            except (IndexError, ValueError):
                continue
            phase = parts[2] if len(parts) > 2 else ""
            bar = "#" * (pct // 4) + "." * (25 - pct // 4)
            text = "    [%s] %3d%%  %s" % (bar, pct, phase[:40])
            sys.stdout.write("\r" + text.ljust(len(last)))
            sys.stdout.flush()
            last = text
        elif line.startswith("##REASON "):
            reason = line[len("##REASON "):]
        elif line.strip():
            tail.append(line)
    rc = p.wait()
    if last:
        sys.stdout.write("\n")
    if rc != 0 or not extracted_already(kit):
        raise SetupError("The game data could not be unpacked.\n\n" +
                         (reason or "\n".join(tail[-8:]) or "extract_assets.py exited with %d" % rc))
    ui.say("    done")


def step_steam(ui, kit, args):
    ui.say()
    ui.say("Step 3 of 3: adding Super Mario 64 DS PC to Steam")
    exe_path = os.path.join(kit, LAUNCHER)
    if not os.path.isfile(exe_path):
        raise SetupError("%s is missing from the kit folder. The kit is incomplete; "
                         "unzip it again." % LAUNCHER)
    roots = find_steam_roots(args.steam_root)
    if not roots:
        raise SetupError("Steam was not found on this machine (no Steam folder with a "
                         "userdata folder in it). Start Steam and sign in once, then run this again.")

    plan = []          # (root, [user ids])
    for root in roots:
        users = steam_users(root)
        if users:
            plan.append((root, users))
        else:
            ui.say("    %s: no signed-in Steam user yet, skipped" % root)
    if not plan:
        raise SetupError("Steam has no signed-in user yet. Start Steam, sign in once, "
                         "then run this again.")
    for root, users in plan:
        ui.say("    Steam folder %s, user(s) %s" % (root, ", ".join(users)))

    # Read everything and work out the changes first, so a run with nothing to change
    # never closes Steam. Steam writes both files when it exits, so once it is closed
    # they are read again and the changes worked out afresh.
    ops = prepare_changes(ui, plan, exe_path, kit, quiet=False)
    stopped_root = None
    if ops and steam_pids():
        if args.dry_run:
            ui.say("    (dry run) Steam is running; a real run would ask to close it now")
        else:
            ok = ui.ask("Steam has to be closed for a moment while the game is added to it.\n"
                        "Close Steam now? It starts again by itself when this is done.")
            if not ok:
                raise SetupError("Steam was left running, so nothing was added.")
            shut_down_steam(ui, plan[0][0])
            stopped_root = plan[0][0]
            ops = prepare_changes(ui, plan, exe_path, kit, quiet=True)

    stamp = time.strftime("%Y%m%d-%H%M%S")
    try:
        for op in ops:
            bak = backup(op["path"], stamp, args.dry_run)
            if args.dry_run:
                ui.say("    (dry run) would " + op["dry"] +
                       ("" if bak is None else ", backup " + os.path.basename(bak)))
                continue
            os.makedirs(os.path.dirname(op["path"]), exist_ok=True)
            write_atomic(op["path"], op["data"])
            with open(op["path"], "rb") as f:
                if f.read() != op["data"]:
                    raise SetupError("%s did not read back the same after writing." % op["path"])
            ui.say("    " + op["done"] +
                   ("" if bak is None else ", old file kept as " + os.path.basename(bak)))
    finally:
        if stopped_root is not None:
            start_steam(ui, stopped_root)
    return any(proton_experimental_installed(root) for root, _ in plan)


def prepare_changes(ui, plan, exe_path, kit, quiet):
    """Read the Steam files and return the writes needed (nothing is written here)."""
    say = (lambda s: None) if quiet else ui.say
    ops = []
    for root, users in plan:
        appids = set()
        for uid in users:
            path = os.path.join(root, "userdata", uid, "config", "shortcuts.vdf")
            raw = None
            if os.path.isfile(path):
                with open(path, "rb") as f:
                    raw = f.read()
                try:
                    data = bvdf_loads(raw)
                except ValueError as e:
                    raise SetupError("Could not read %s (%s). It was left as it is." % (path, e))
                if bvdf_dumps(data) != raw:
                    raise SetupError("%s did not read back byte for byte, so it was left "
                                     "alone to be safe." % path)
            else:
                data = [["shortcuts", T_MAP, []]]
            appid, action, dups = upsert_shortcut(data, exe_path, kit)
            appids.add(appid)
            out = bvdf_dumps(data)
            bvdf_loads(out)                         # must parse before it is written
            if out == raw:
                say("    user %s: already in Steam (app id %d), nothing to change" % (uid, appid))
            else:
                ops.append({"path": path, "data": out,
                            "dry": "%s the shortcut for user %s, app id %d" %
                                   ("add" if action == "added" else "update", uid, appid),
                            "done": "user %s: shortcut %s (app id %d)" % (uid, action, appid)})
            if dups:
                say("    user %s: %d more old entr%s for this game left as they are" %
                    (uid, dups, "y" if dups == 1 else "ies"))

        cfg_path = os.path.join(root, "config", "config.vdf")
        text = None
        if os.path.isfile(cfg_path):
            with open(cfg_path, "r", encoding="utf-8", errors="surrogateescape") as f:
                text = f.read()
            try:
                cfg = tvdf_loads(text)
            except ValueError as e:
                raise SetupError("Could not read %s (%s). It was left as it is; set the "
                                 "compatibility tool by hand (step 4 below)." % (cfg_path, e))
        else:
            cfg = []
        ids = sorted(appids)
        for a in ids:
            set_compat_tool(cfg, a)
        new_text = tvdf_dumps(cfg)
        check = tvdf_loads(new_text)
        for a in ids:
            if read_compat_tool(check, a) != COMPAT_TOOL:
                raise SetupError("Setting the compatibility tool did not read back.")
        if new_text != text:
            names = ", ".join(str(a) for a in ids)
            ops.append({"path": cfg_path, "data": new_text.encode("utf-8", "surrogateescape"),
                        "dry": "set Proton Experimental for app id(s) %s in %s" % (names, cfg_path),
                        "done": "Proton Experimental set for app id(s) %s" % names})
        else:
            say("    Proton Experimental was already set")
    return ops


def main(argv=None):
    ap = argparse.ArgumentParser(description="Steam Deck Setup for the SM64DS PC port.")
    ap.add_argument("--dry-run", action="store_true")
    ap.add_argument("--steam-root", action="append")
    ap.add_argument("--kit")
    ap.add_argument("--yes", action="store_true")
    ap.add_argument("--no-gui", action="store_true")
    ap.add_argument("--no-pause", action="store_true")
    args = ap.parse_args(argv)

    kit = os.path.abspath(args.kit or os.path.dirname(os.path.abspath(__file__)))
    ui = UI(gui=not args.no_gui and not args.dry_run, assume_yes=args.yes)
    ui.say("Super Mario 64 DS PC: Steam Deck Setup%s" % ("  (DRY RUN: nothing is written)"
                                                         if args.dry_run else ""))
    ui.say("Kit folder: " + kit)
    ui.say()
    rc = 0
    try:
        if not step_rom(ui, kit):
            rc = 1
        else:
            step_extract(ui, kit, args.dry_run)
            proton = step_steam(ui, kit, args)
            msg = "Done. Switch to Gaming Mode and find Super Mario 64 DS PC in your library."
            if not proton:
                msg += ("\n\nProton Experimental is not installed yet. Before you start the game: "
                        "in Steam, open the Library, set the filter to Tools, find Proton "
                        "Experimental and install it. The game is already set to use it.")
            if args.dry_run:
                msg = "(dry run) " + msg
            ui.info(msg)
    except SetupError as e:
        rc = 1
        ui.info("Setup stopped: %s\n\n%s" % (e, MANUAL_STEPS), error=True)
    except Exception as e:          # anything unexpected: never leave them with nothing
        rc = 1
        ui.info("Setup stopped on an unexpected problem (%s: %s).\n\n%s" %
                (type(e).__name__, e, MANUAL_STEPS), error=True)
    if not args.no_pause and sys.stdin is not None and sys.stdin.isatty():
        try:
            input("Press Enter to close this window.")
        except EOFError:
            pass
    return rc


if __name__ == "__main__":
    sys.exit(main())
