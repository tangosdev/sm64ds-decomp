#!/usr/bin/env python3
"""Offline tests for steam_deck_setup.py against a fake Steam folder.

    python3 tools/portable_kit/test_steam_deck_setup.py

Never looks at a real Steam install: every Steam folder used here is made in a
temporary directory and passed with --steam-root. Not shipped in the kit.
"""
import hashlib
import io
import os
import shutil
import struct
import sys
import tempfile
import zlib
from contextlib import redirect_stdout

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import steam_deck_setup as sds  # noqa: E402


# An encoder written independently of the one under test, laid out the way
# Steam itself writes shortcuts.vdf (field names and order from a Steam client
# file; values made up).
def enc_str(k, v):
    return b"\x01" + k.encode() + b"\0" + v.encode() + b"\0"


def enc_int(k, v):
    return b"\x02" + k.encode() + b"\0" + struct.pack("<I", v & 0xFFFFFFFF)


def enc_map(k, body):
    return b"\x00" + k.encode() + b"\0" + body + b"\x08"


def steam_entry(idx, appid, name, exe, start, tags=()):
    body = b"".join([
        enc_int("appid", appid), enc_str("AppName", name), enc_str("Exe", exe),
        enc_str("StartDir", start), enc_str("icon", ""), enc_str("ShortcutPath", ""),
        enc_str("LaunchOptions", ""), enc_int("IsHidden", 0), enc_int("AllowDesktopConfig", 1),
        enc_int("AllowOverlay", 1), enc_int("OpenVR", 0), enc_int("Devkit", 0),
        enc_str("DevkitGameID", ""), enc_int("DevkitOverrideAppID", 0),
        enc_int("LastPlayTime", 1731400000), enc_str("FlatpakAppID", ""),
        enc_map("tags", b"".join(enc_str(str(i), t) for i, t in enumerate(tags))),
    ])
    return enc_map(str(idx), body)


def shortcuts_file(*entries):
    return enc_map("shortcuts", b"".join(entries)) + b"\x08"


CONFIG_VDF = '''"InstallConfigStore"
{
\t"Software"
\t{
\t\t"Valve"
\t\t{
\t\t\t"Steam"
\t\t\t{
\t\t\t\t"AutoUpdateWindowEnabled"\t\t"0"
\t\t\t\t"CompatToolMapping"
\t\t\t\t{
\t\t\t\t\t"0"
\t\t\t\t\t{
\t\t\t\t\t\t"name"\t\t"proton_9"
\t\t\t\t\t\t"config"\t\t""
\t\t\t\t\t\t"priority"\t\t"75"
\t\t\t\t\t}
\t\t\t\t\t"2989342463"
\t\t\t\t\t{
\t\t\t\t\t\t"name"\t\t"GE-Proton9-20"
\t\t\t\t\t\t"config"\t\t""
\t\t\t\t\t\t"priority"\t\t"250"
\t\t\t\t\t}
\t\t\t\t}
\t\t\t\t"SentryFile"\t\t"/home/deck/.local/share/Steam/ssfn_quoted\\"x\\\\y"
\t\t\t}
\t\t}
\t}
\t"Music"
\t{
\t\t"LocalLibraryRoot"\t\t""
\t}
}
'''

LIBRARYFOLDERS = '''"libraryfolders"
{
\t"0"
\t{
\t\t"path"\t\t"%s"
\t\t"label"\t\t""
\t\t"apps"
\t\t{
\t\t\t"1493710"\t\t"1234"
\t\t}
\t}
\t"1"
\t{
\t\t"path"\t\t"%s"
\t}
}
'''

FAILS = []


def check(cond, what):
    print(("PASS  " if cond else "FAIL  ") + what)
    if not cond:
        FAILS.append(what)


def tree_hash(root):
    h = hashlib.sha256()
    for d, dirs, files in sorted(os.walk(root)):
        dirs.sort()
        for f in sorted(files):
            p = os.path.join(d, f)
            h.update(os.path.relpath(p, root).encode())
            with open(p, "rb") as fh:
                h.update(fh.read())
    return h.hexdigest()


def run(argv):
    buf = io.StringIO()
    with redirect_stdout(buf):
        rc = sds.main(argv + ["--no-gui", "--no-pause"])
    return rc, buf.getvalue()


def make_kit(base, name="SM64DS-PC 1.0.3", rom=True, extracted=True):
    kit = os.path.join(base, name)
    os.makedirs(os.path.join(kit, "PLACE YOUR ROM HERE"))
    open(os.path.join(kit, sds.LAUNCHER), "wb").write(b"MZ")
    shutil.copy(os.path.join(HERE, "extract_assets.py"), kit)
    if rom:
        open(os.path.join(kit, "PLACE YOUR ROM HERE", "sm64ds.nds"), "wb").write(b"\0" * 16)
    if extracted:
        for f in sds.EXTRACTED_FILES:
            p = os.path.join(kit, f)
            os.makedirs(os.path.dirname(p), exist_ok=True)
            open(p, "wb").write(b"x")
    return kit


def make_steam(base, proton=True):
    root = os.path.join(base, "Steam")
    lib2 = os.path.join(base, "sdcard", "SteamLibrary")
    os.makedirs(os.path.join(root, "config"))
    os.makedirs(os.path.join(root, "steamapps"))
    os.makedirs(os.path.join(root, "userdata", "0", "config"))
    os.makedirs(os.path.join(root, "userdata", "12345678", "config"))
    os.makedirs(os.path.join(root, "userdata", "87654321", "config"))
    open(os.path.join(root, "userdata", "0", "config", "shortcuts.vdf"), "wb").write(
        shortcuts_file())
    existing = shortcuts_file(
        steam_entry(0, 0x8F3A1C22, "Firefox", '"/usr/bin/flatpak"', '"/usr/bin/"', ("favorite",)),
        steam_entry(1, 0x9ABCDEF0, "RetroArch", '"/home/deck/Applications/RetroArch.AppImage"',
                    '"/home/deck/Applications/"'))
    open(os.path.join(root, "userdata", "12345678", "config", "shortcuts.vdf"), "wb").write(existing)
    with open(os.path.join(root, "config", "config.vdf"), "w", newline="\n") as f:
        f.write(CONFIG_VDF)
    with open(os.path.join(root, "steamapps", "libraryfolders.vdf"), "w", newline="\n") as f:
        f.write(LIBRARYFOLDERS % (root.replace("\\", "\\\\"), lib2.replace("\\", "\\\\")))
    if proton:
        p = os.path.join(lib2, "steamapps", "common", sds.COMPAT_TOOL_DIR)
        os.makedirs(p)
        open(os.path.join(p, "proton"), "w").write("#!/usr/bin/env python3\n")
    return root, existing


def entries_named(path):
    root = sds.bvdf_loads(open(path, "rb").read())
    out = []
    for e in sds.bvdf_get(root, "shortcuts")[2]:
        n = sds.bvdf_get(e[2], "AppName")
        if n and n[2] == sds.APP_NAME:
            out.append({k: v for k, t, v in e[2]})
    return out


def main():
    print("python", sys.version.split()[0], "on", sys.platform)

    # 1. Binary VDF round trip, byte for byte.
    raw = shortcuts_file(
        steam_entry(0, 0x8F3A1C22, "Firefox", '"/usr/bin/flatpak"', '"/usr/bin/"', ("favorite", "x")),
        steam_entry(1, 0x80000001, "Caf\u00e9 \u30b2\u30fc\u30e0", '"/a b/c.exe"', '"/a b/"'))
    check(sds.bvdf_dumps(sds.bvdf_loads(raw)) == raw, "shortcuts.vdf round trip is byte-identical")
    check(sds.bvdf_dumps(sds.bvdf_loads(shortcuts_file())) == shortcuts_file(),
          "empty shortcuts.vdf round trip")
    try:
        sds.bvdf_loads(raw[:-3])
        check(False, "truncated shortcuts.vdf is refused")
    except ValueError:
        check(True, "truncated shortcuts.vdf is refused")

    # 2. App id formula, recomputed here independently.
    exe = '"/home/deck/Games/SM64DS-PC/SM64DSLauncher.exe"'
    want = (zlib.crc32((exe + "Super Mario 64 DS PC").encode()) & 0xFFFFFFFF) | 0x80000000
    got = sds.shortcut_appid(exe, "Super Mario 64 DS PC")
    check(got == want and got >= 0x80000000, "app id = crc32(Exe+AppName)|0x80000000 (%d)" % got)
    print("      example: Exe %s -> app id %d (0x%08X), legacy 64-bit id %d" %
          (exe, got, got, (got << 32) | 0x02000000))

    # 3. Text VDF: Steam-formatted config.vdf reads and writes back unchanged.
    cfg = sds.tvdf_loads(CONFIG_VDF)
    check(sds.tvdf_dumps(cfg) == CONFIG_VDF, "config.vdf round trip is text-identical")
    check(sds.read_compat_tool(cfg, 2989342463) == "GE-Proton9-20", "reads an existing mapping")

    base = tempfile.mkdtemp(prefix="sds-test-")
    try:
        kit = make_kit(base)
        root, existing = make_steam(base)
        u1 = os.path.join(root, "userdata", "12345678", "config", "shortcuts.vdf")
        u2 = os.path.join(root, "userdata", "87654321", "config", "shortcuts.vdf")
        u0 = os.path.join(root, "userdata", "0", "config", "shortcuts.vdf")
        cfgp = os.path.join(root, "config", "config.vdf")

        # 4. Dry run writes nothing.
        before = tree_hash(base)
        rc, out = run(["--kit", kit, "--steam-root", root, "--dry-run"])
        check(rc == 0, "dry run exits 0")
        check(tree_hash(base) == before, "dry run changed no file")
        check("would add the shortcut" in out and "would set Proton Experimental" in out,
              "dry run says what it would do")
        print("      --- dry run output ---\n" + "\n".join("      " + l for l in out.splitlines()))

        # 5. Real run.
        rc, out = run(["--kit", kit, "--steam-root", root, "--yes"])
        check(rc == 0, "first run exits 0")
        print("      --- first run output ---\n" + "\n".join("      " + l for l in out.splitlines()))
        e1, e2 = entries_named(u1), entries_named(u2)
        check(len(e1) == 1 and len(e2) == 1, "one entry for each real user")
        exe_field = '"%s"' % os.path.join(kit, sds.LAUNCHER)
        appid = sds.shortcut_appid(exe_field, sds.APP_NAME)
        check(e1[0]["Exe"] == exe_field and e1[0]["StartDir"] == '"%s/"' % kit,
              "Exe and StartDir point at the kit (StartDir = kit folder)")
        check(struct.unpack("<I", e1[0]["appid"])[0] == appid, "entry carries the computed app id")
        all1 = sds.bvdf_loads(open(u1, "rb").read())
        names = [sds.bvdf_get(e[2], "AppName")[2] for e in sds.bvdf_get(all1, "shortcuts")[2]]
        check(names == ["Firefox", "RetroArch", sds.APP_NAME], "existing shortcuts kept, in order")
        check(open(u1, "rb").read().startswith(existing[:-2]),
              "existing bytes untouched, entry appended")
        check(open(u0, "rb").read() == shortcuts_file(), "userdata/0 left alone")
        baks = [f for f in os.listdir(os.path.dirname(u1)) if f.startswith("shortcuts.vdf.bak-")]
        check(len(baks) == 1 and open(os.path.join(os.path.dirname(u1), baks[0]), "rb").read() == existing,
              "shortcuts.vdf backed up before writing")
        check(not [f for f in os.listdir(os.path.dirname(u2)) if ".bak-" in f],
              "no backup made where there was no file")
        cfg2 = sds.tvdf_loads(open(cfgp).read())
        check(sds.read_compat_tool(cfg2, appid) == "proton_experimental", "Proton Experimental mapped")
        check(sds.read_compat_tool(cfg2, 2989342463) == "GE-Proton9-20", "other mapping kept")
        check(sds.read_compat_tool(cfg2, 0) == "proton_9", "global default kept")
        cbaks = [f for f in os.listdir(os.path.dirname(cfgp)) if f.startswith("config.vdf.bak-")]
        check(len(cbaks) == 1 and open(os.path.join(os.path.dirname(cfgp), cbaks[0])).read() == CONFIG_VDF,
              "config.vdf backed up before writing")
        check("Done. Switch to Gaming Mode and find Super Mario 64 DS PC in your library." in out,
              "ends with the plain Done message")
        check("not installed" not in out, "no install hint when Proton Experimental is there")
        sentry = sds.tvdf_get(sds.tvdf_get(sds.tvdf_get(sds.tvdf_get(sds.tvdf_get(
            cfg2, "InstallConfigStore")[1], "Software")[1], "Valve")[1], "Steam")[1], "SentryFile")[1]
        check(sentry.endswith('ssfn_quoted"x\\y'), "escaped strings survive")

        # 6. Second run: nothing new.
        snap = {p: open(p, "rb").read() for p in (u1, u2, cfgp)}
        import time as _t
        _t.sleep(1.1)                   # a new backup stamp if anything were rewritten
        rc, out = run(["--kit", kit, "--steam-root", root, "--yes"])
        check(rc == 0 and len(entries_named(u1)) == 1 and len(entries_named(u2)) == 1,
              "second run adds no second entry")
        check(all(open(p, "rb").read() == b for p, b in snap.items()), "second run: files identical")
        check("already set" in out, "second run leaves config.vdf alone")
        check(out.count("nothing to change") == 2, "second run says both users already have it")
        nb = [f for d in (os.path.dirname(u1), os.path.dirname(u2), os.path.dirname(cfgp))
              for f in os.listdir(d) if ".bak-" in f]
        check(len(nb) == 2, "second run makes no new backups")

        # 7. Kit moved: same entry updated, app id kept.
        kit2 = os.path.join(base, "moved kit")
        shutil.move(kit, kit2)
        rc, out = run(["--kit", kit2, "--steam-root", root, "--yes"])
        e1 = entries_named(u1)
        check(rc == 0 and len(e1) == 1 and e1[0]["Exe"] == '"%s"' % os.path.join(kit2, sds.LAUNCHER),
              "moved kit updates the one entry")
        check(struct.unpack("<I", e1[0]["appid"])[0] == appid, "moved kit keeps the app id")
        kit = kit2

        # 8. Corrupt shortcuts.vdf: refused, file left as it is, manual steps shown.
        bad = b"\x00shortcuts\x00\x09junk"
        open(u2, "wb").write(bad)
        rc, out = run(["--kit", kit, "--steam-root", root, "--yes"])
        check(rc == 1 and open(u2, "rb").read() == bad and "by hand" in out,
              "unreadable shortcuts.vdf: stops, leaves it, prints manual steps")
        open(u2, "wb").write(snap[u2])
    finally:
        shutil.rmtree(base, ignore_errors=True)

    # 9. No ROM: a clear message, nothing written.
    base = tempfile.mkdtemp(prefix="sds-test-")
    try:
        kit = make_kit(base, rom=False)
        root, _ = make_steam(base)
        before = tree_hash(base)
        rc, out = run(["--kit", kit, "--steam-root", root, "--yes"])
        check(rc == 1 and "No .nds file was found" in out and "PLACE YOUR ROM HERE" in out,
              "no ROM: says put it in PLACE YOUR ROM HERE")
        check(tree_hash(base) == before, "no ROM: nothing written")
    finally:
        shutil.rmtree(base, ignore_errors=True)

    # 10. Proton Experimental missing: entry still added, install hint given.
    base = tempfile.mkdtemp(prefix="sds-test-")
    try:
        kit = make_kit(base)
        root, _ = make_steam(base, proton=False)
        rc, out = run(["--kit", kit, "--steam-root", root, "--yes"])
        check(rc == 0 and "Library, set the filter to Tools" in out, "no Proton: install hint")
        check(len(entries_named(os.path.join(root, "userdata", "12345678", "config",
                                             "shortcuts.vdf"))) == 1, "no Proton: entry still added")
    finally:
        shutil.rmtree(base, ignore_errors=True)

    # 11. Extraction: progress is shown and a failure is reported (fake extractor).
    base = tempfile.mkdtemp(prefix="sds-test-")
    try:
        kit = make_kit(base, extracted=False)
        root, _ = make_steam(base)
        files = repr(list(sds.EXTRACTED_FILES))
        with open(os.path.join(kit, "extract_assets.py"), "w") as f:
            f.write("import os,sys\nfor p in (0,40,100):\n print('##PROGRESS %%d Writing files' %% p, flush=True)\n"
                    "d=sys.argv[sys.argv.index('--destination')+1]\n"
                    "for r in %s:\n p=os.path.join(d,r); os.makedirs(os.path.dirname(p),exist_ok=True); open(p,'w').write('x')\n" % files)
        rc, out = run(["--kit", kit, "--steam-root", root, "--yes"])
        check(rc == 0 and "100%" in out, "extraction runs with a progress bar")
        rc, out = run(["--kit", kit, "--steam-root", root, "--yes"])
        check("already unpacked, skipping" in out, "extraction skipped once done")
        os.remove(os.path.join(kit, sds.EXTRACTED_FILES[0]))
        with open(os.path.join(kit, "extract_assets.py"), "w") as f:
            f.write("print('##REASON That is not a Super Mario 64 DS dump.'); raise SystemExit(1)\n")
        rc, out = run(["--kit", kit, "--steam-root", root, "--yes"])
        check(rc == 1 and "not a Super Mario 64 DS dump" in out and "by hand" in out,
              "extraction failure: reason and manual steps")
    finally:
        shutil.rmtree(base, ignore_errors=True)

    # 12a. No config.vdf yet: it is made with just the mapping.
    base = tempfile.mkdtemp(prefix="sds-test-")
    try:
        kit = make_kit(base)
        root, _ = make_steam(base)
        os.remove(os.path.join(root, "config", "config.vdf"))
        rc, out = run(["--kit", kit, "--steam-root", root, "--yes"])
        cfg = sds.tvdf_loads(open(os.path.join(root, "config", "config.vdf")).read())
        appid = sds.shortcut_appid('"%s"' % os.path.join(kit, sds.LAUNCHER), sds.APP_NAME)
        check(rc == 0 and sds.read_compat_tool(cfg, appid) == "proton_experimental",
              "missing config.vdf: made with the mapping")
    finally:
        shutil.rmtree(base, ignore_errors=True)

    # 12. No Steam user signed in yet.
    base = tempfile.mkdtemp(prefix="sds-test-")
    try:
        kit = make_kit(base)
        root = os.path.join(base, "EmptySteam")
        os.makedirs(os.path.join(root, "userdata", "0"))
        rc, out = run(["--kit", kit, "--steam-root", root, "--yes"])
        check(rc == 1 and "no signed-in user" in out, "no Steam user: says sign in first")
    finally:
        shutil.rmtree(base, ignore_errors=True)

    print()
    print("%d failed" % len(FAILS) if FAILS else "ALL PASSED")
    return 1 if FAILS else 0


if __name__ == "__main__":
    sys.exit(main())
