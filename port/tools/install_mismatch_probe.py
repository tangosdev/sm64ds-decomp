#!/usr/bin/env python3
"""Prove the version-mixed-install guards, end to end, on the real window.

WHAT THIS IS FOR
================
An exe and the asset folder it reads can belong to different builds, and until
2026-08-26 nothing in the port could tell. Both existing checks are blind to it
by construction:

  * romdata.manifest ships BESIDE romdata.bin, so any (blob, manifest) pair
    validates itself. The check answers "is this blob intact", never "is this
    blob MINE".
  * every field in the lk7 save-state header is an EXE-side fact -- build,
    image base, arena base, section bounds -- so a save state made under one
    asset folder's game data matches all of them under another's.

Meanwhile port_romdata_load_all's per-part offsets are baked into the exe, so
another build's blob fills every ROM table from the wrong slice in silence, and
the world dies a few seconds later with level data that cannot exist.

Two guards closed it (hal/romdata_loader.cpp, hal/lk7_persist.cpp). This drives
them against the shipped window and proves BOTH directions: with the guard off
the world faults, with the guard on it refuses politely and boots fresh.

THIS FAILURE HAS NOT BEEN SEEN IN THE FIELD. It was found by reading the loader
during the 2026-08-26 crash burst, and it is NOT what that burst was -- that
install's data matched its exe, and its real defect (a save state its own build
could not reload) is untouched by either guard and has its own lane. Written
down here because a probe whose docstring claims a victim it does not have is a
probe the next reader quietly stops believing.

THE DOCTORED ASSET FOLDER IS A REAL ONE, NOT A CORRUPTED FILE
=============================================================
A flipped byte would be caught by checks that already existed. The dangerous
folder is the one that is INTERNALLY PERFECT and belongs to another build, so
that is what this makes: the emitters' own .blobpart pieces, reconsolidated by
port/tools/romblob.py with one early part left out. The result is a blob with a
different length, every later part at a shifted offset, and a manifest that
describes it exactly -- which is precisely what an older release's folder is.
The exe's own part offsets are baked in at build time and no longer point at the
right bytes, and nothing but the new stamp can tell.

    python port/tools/install_mismatch_probe.py [repo-root] [--level N]
                                                [--frames N] [--keep]

--keep leaves the doctored pair and the artifacts in place for inspection.
Exit 0 if every arm landed where it should, 1 on the first that did not.

THE REAL PAIR IS PUT BACK. The doctoring swaps build/assets/romdata.bin and
.manifest in place (much cheaper than a second copy of the tree, and the guard
compares identity, not paths); the originals are moved aside as .realbak and
restored in a finally. If this script is killed between the swap and the
restore, `git status` will not show it -- build/assets is generated -- so the
recovery is to rebuild, or to move the two .realbak files back by hand.
"""

import hashlib
import json
import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

TIMEOUT = 600


def spawn(exe, cwd, env, timeout=TIMEOUT):
    """Run walk_window with the desk left alone.

    Three separate things have to be asked for or a scripted run steals focus,
    and battery.py / savestate_soak.py ask for all three: SM64DS_NO_FOCUS so the
    window is created unactivated, CREATE_NO_WINDOW so a console-subsystem child
    does not conjure a console, and STARTUPINFO SW_SHOWMINNOACTIVE so the window
    it does open starts minimized. Tango is at the desk while these run.
    """
    e = dict(os.environ, SM64DS_NO_FOCUS="1", SM64DS_NO_DIALOG="1")
    for k in ("SM64DS_SKIP_CLASS", "SM64DS_SCENE", "SM64DS_SS_DISKLOAD",
              "SM64DS_SS_DISK", "SM64DS_ROMDATA_NO_GUARD",
              "SM64DS_SAVESTATE_NO_GUARD"):
        e.pop(k, None)
    e.update(env)
    si = None
    if hasattr(subprocess, "STARTUPINFO"):
        si = subprocess.STARTUPINFO()
        si.dwFlags |= subprocess.STARTF_USESHOWWINDOW
        si.wShowWindow = 7  # SW_SHOWMINNOACTIVE
    try:
        r = subprocess.run([str(exe)], cwd=str(cwd), env=e, timeout=timeout,
                           capture_output=True, text=True, errors="replace",
                           creationflags=getattr(subprocess,
                                                 "CREATE_NO_WINDOW", 0),
                           startupinfo=si)
        return r.returncode, (r.stdout or "") + (r.stderr or "")
    except subprocess.TimeoutExpired as t:
        out = (t.stdout or b"") + (t.stderr or b"")
        if isinstance(out, bytes):
            out = out.decode("utf-8", "replace")
        return None, out


def make_other_build_blob(build, out_bin, out_manifest, drop_index=0):
    """Reconsolidate the emitters' parts with one part left out.

    Uses port/tools/romblob.py itself, so the pair it writes is self-consistent
    by construction -- the same tool, the same code path, a different input set.
    Returns (bytes, sha, dropped_tag).
    """
    host_src = build / "host-src"
    maps = sorted(host_src.glob("*.blobmap"),
                  key=lambda p: json.loads(p.read_text())["tag"])
    if len(maps) < 3:
        sys.exit(f"install_mismatch_probe: only {len(maps)} .blobmap parts "
                 f"under {host_src}; build with -DPORT_ROM_CLEAN=ON first")
    dropped = json.loads(maps[drop_index].read_text())["tag"]
    keep = [m for i, m in enumerate(maps) if i != drop_index]
    scratch = build / "mixed_install_probe"
    scratch.mkdir(parents=True, exist_ok=True)
    rc = subprocess.run(
        [sys.executable,
         str(Path(__file__).with_name("romblob.py")),
         str(out_bin), str(out_manifest), str(scratch / "dispatch.c"),
         *[str(m) for m in keep]],
        capture_output=True, text=True)
    if rc.returncode != 0:
        sys.exit("install_mismatch_probe: romblob failed:\n"
                 + rc.stdout + rc.stderr)
    raw = out_bin.read_bytes()
    return len(raw), hashlib.sha256(raw).hexdigest(), dropped


def poke_savestate_field(path, offset, value):
    with open(path, "r+b") as f:
        f.seek(offset)
        f.write(value)


def main():
    argv = sys.argv[1:]
    level, frames, keep = "1", "240", False
    for flag, setter in (("--level", "level"), ("--frames", "frames")):
        if flag in argv:
            i = argv.index(flag)
            val = argv[i + 1]
            del argv[i:i + 2]
            if setter == "level":
                level = val
            else:
                frames = val
    if "--keep" in argv:
        keep = True
        argv.remove("--keep")
    root = Path(argv[0]) if argv else Path(__file__).resolve().parents[2]
    build = root / "build" / "port"
    exe = build / "walk_window.exe"
    assets = root / "build" / "assets"
    real_bin, real_man = assets / "romdata.bin", assets / "romdata.manifest"
    if not exe.exists():
        sys.exit(f"install_mismatch_probe: no {exe}; build first")
    if not real_bin.exists():
        sys.exit(f"install_mismatch_probe: no {real_bin}; build with "
                 "-DPORT_ROM_CLEAN=ON")
    # REFUSE BY NAME rather than letting arm 1 fail as "boots clean rc=2".
    # A fresh worktree has no asset catalog, the ROM-clean exe refuses to start
    # without one, and every arm then reports a number that means nothing. This
    # cost a reviewer twenty minutes; it costs one stat call.
    missing = [n for n in ("files.tsv", "handles.tsv", "nitrofs.tsv")
               if not (assets / n).exists()]
    if missing:
        sys.exit(f"install_mismatch_probe: {assets} has no "
                 + ", ".join(missing)
                 + ".\n  The game refuses to start without the asset catalog, "
                   "so every arm below would\n  fail as 'boots clean rc=2' and "
                   "tell you nothing. Generate it first:\n"
                   "    python tools/asset_catalog.py generate <rom.nds>")

    real_len = real_bin.stat().st_size
    real_sha = hashlib.sha256(real_bin.read_bytes()).hexdigest()
    print(f"real romdata.bin: {real_len} bytes sha {real_sha[:16]}")

    other_bin = build / "mixed_install_probe" / "romdata.bin"
    other_man = build / "mixed_install_probe" / "romdata.manifest"
    other_bin.parent.mkdir(parents=True, exist_ok=True)
    other_len, other_sha, dropped = make_other_build_blob(
        build, other_bin, other_man)
    print(f"doctored 'other build' romdata.bin: {other_len} bytes sha "
          f"{other_sha[:16]}  (part '{dropped}' left out; every later part "
          f"shifts by {real_len - other_len} bytes)")

    state = build / "savestate.bin"
    if state.exists():
        state.unlink()

    bak_bin = assets / "romdata.bin.realbak"
    bak_man = assets / "romdata.manifest.realbak"
    startup_err = build / "startup_error.txt"
    fails = []

    def check(name, cond, detail=""):
        print(f"  {'ok  ' if cond else 'FAIL'}  {name}"
              + (f"   {detail}" if detail and not cond else ""))
        if not cond:
            fails.append(name)

    def use_doctored():
        shutil.move(str(real_bin), str(bak_bin))
        shutil.move(str(real_man), str(bak_man))
        shutil.copy2(str(other_bin), str(real_bin))
        shutil.copy2(str(other_man), str(real_man))

    def use_real():
        if bak_bin.exists():
            if real_bin.exists():
                real_bin.unlink()
            shutil.move(str(bak_bin), str(real_bin))
        if bak_man.exists():
            if real_man.exists():
                real_man.unlink()
            shutil.move(str(bak_man), str(real_man))

    base_env = {"SM64DS_ASSET_ROOT": str(root),
                "SM64DS_WINDOW_SELFTEST": frames,
                "SM64DS_LEVEL": level}
    logs = {}
    try:
        # ---- ARM 1: the control. The real pair must be SILENT. --------------
        print("\nARM 1  real asset folder, guards on -- must be silent")
        rc, log = spawn(exe, build, base_env)
        logs["arm1_real_control"] = log
        check("boots clean", rc == 0, f"rc={rc}")
        check("no mismatch warning", "DIFFERENT BUILD" not in log.upper())
        check("romdata loaded", "[romdata] loaded" in log)

        # ---- ARM 2: pre-fix behaviour. Wrong slices, then a fault. ----------
        print("\nARM 2  other build's asset folder, romdata guard OFF "
              "(= the pre-fix exe)")
        use_doctored()
        if startup_err.exists():
            startup_err.unlink()
        rc, log = spawn(exe, build,
                        dict(base_env, SM64DS_ROMDATA_NO_GUARD="1",
                             SM64DS_FAULTS_FATAL="1"))
        logs["arm2_prefix_crash"] = log
        bad = (rc not in (0, None)) or "ASSERT" in log or "FAULT" in log
        check("loads the wrong blob and the world goes bad",
              bad, f"rc={rc} -- the doctored blob did not break anything; "
                   "try a different --level or drop a bigger part")
        check("the warning at least NAMES it",
              "DIFFERENT BUILD" in log.upper())

        # ---- ARM 3: the guard. Polite refusal, no crash. --------------------
        print("\nARM 3  same folder, romdata guard ON -- must refuse politely")
        if startup_err.exists():
            startup_err.unlink()
        rc, log = spawn(exe, build, base_env)
        logs["arm3_romdata_refused"] = log
        check("exits 2 (a refusal, not a crash)", rc == 2, f"rc={rc}")
        check("names both blobs",
              other_sha[:16] in log and real_sha[:16] in log)
        check("no fault, no assert",
              "FAULT" not in log and "ASSERT" not in log)
        check("wrote startup_error.txt for the launcher", startup_err.exists())
        if startup_err.exists():
            txt = startup_err.read_text(errors="replace")
            logs["arm3_startup_error.txt"] = txt
            check("...in plain language",
                  "different version of the game" in txt)

        # ---- ARM 4: build an OLD BUILD'S save state -------------------------
        # The obvious way to get one -- boot the doctored folder and save --
        # does not work, and arm 2 is why: a world built from another build's
        # ROM tables dies before it reaches any save frame. That IS the point
        # of arm 2, and it means an old build's state has to be synthesised.
        #
        # So: save a clean state under the REAL folder, then make it into what
        # an older build's savestate.bin actually is, in the two ways that
        # matter and no others.
        #
        #   1. STAMP: the romdata identity field says another blob. This is
        #      what the guard reads, and it is the only field that changes --
        #      gittip, image base, arena base and every section bound still
        #      agree, which is the whole shape format 2 could not express.
        #   2. BODY: the captured arena is rotated by four bytes, so every
        #      pointer in the restored object graph reads its neighbour's word.
        #      This is what makes the file DANGEROUS rather than merely
        #      mislabelled, and it is the honest stand-in for a state whose
        #      hosted layout moved -- the .dsstate base has shifted three times
        #      this week alone, and every one of those shifts has this effect
        #      on a state written before it.
        #
        # A file that is only mislabelled would prove the guard fires and prove
        # nothing about what the guard is FOR. A file that is only mis-bodied
        # would crash both builds. It takes both to show the pair.
        use_real()
        print("\nARM 4  make an older build's save state (clean save, then "
              "stamp + body aged)")
        rc, log = spawn(exe, build,
                        dict(base_env, SM64DS_SS_SAVE="60", SM64DS_SS_DISK="1"))
        logs["arm4_clean_save_then_age"] = log
        check("savestate.bin written by a clean run", state.exists())
        if not state.exists():
            raise SystemExit("install_mismatch_probe: no state to age; "
                             "see arm4_clean_save_then_age.log")
        raw = bytearray(state.read_bytes())
        HDR, SHA_OFF, ARENA_OFF = 717, 132, 717
        stamped_before = bytes(raw[SHA_OFF:SHA_OFF + 64]).decode("ascii",
                                                                "replace")
        check("the clean state carries THIS build's data stamp",
              stamped_before == real_sha,
              f"stamped {stamped_before[:16]}, real {real_sha[:16]}")
        arena_len = int.from_bytes(raw[92:100], "little")   # Header.arena_size
        check("header's arena_size is sane",
              0 < arena_len <= len(raw) - HDR, f"arena_size={arena_len}")
        raw[SHA_OFF:SHA_OFF + 64] = other_sha.encode("ascii")
        body = raw[ARENA_OFF:ARENA_OFF + arena_len]
        raw[ARENA_OFF:ARENA_OFF + arena_len] = body[4:] + body[:4]
        state.write_bytes(bytes(raw))
        print(f"       aged: stamp -> {other_sha[:16]}, arena rotated 4 bytes "
              f"({arena_len} bytes of object graph)")

        # ---- ARM 5: pre-fix behaviour. The state loads and poisons the world.
        print("\nARM 5  that state + savestate guard OFF (= the pre-fix exe): "
              "the world goes bad")
        rc, log = spawn(exe, build,
                        dict(base_env, SM64DS_SS_DISKLOAD="1",
                             SM64DS_SAVESTATE_NO_GUARD="1",
                             SM64DS_FAULTS_FATAL="1"))
        logs["arm5_prefix_loads_poisoned_state"] = log
        check("the poisoned state is loaded anyway",
              "loaded disk state" in log or "guard OVERRIDDEN" in log)
        check("and the world goes bad",
              (rc not in (0, None)) or "ASSERT" in log or "FAULT" in log,
              f"rc={rc}")

        # ---- ARM 6: the guard. Refused, fresh boot, file untouched. ---------
        print("\nARM 6  the same state, savestate guard ON: refused, "
              "boots fresh")
        size_before = state.stat().st_size
        rc, log = spawn(exe, build, dict(base_env, SM64DS_SS_DISKLOAD="1"))
        logs["arm6_savestate_refused"] = log
        check("refused, and says it is the game data",
              "DIFFERENT GAME DATA" in log)
        check("names both stamps",
              other_sha[:16] in log and real_sha[:16] in log)
        check("booted fresh instead (exit 0, no fault, no assert)",
              rc == 0 and "FAULT" not in log and "ASSERT" not in log,
              f"rc={rc}")
        check("the state file was left untouched",
              state.exists() and state.stat().st_size == size_before)
    finally:
        use_real()
        if not keep:
            if state.exists():
                state.unlink()
            shutil.rmtree(build / "mixed_install_probe", ignore_errors=True)

    outdir = build / "mixed_install_probe_logs"
    outdir.mkdir(parents=True, exist_ok=True)
    for name, text in logs.items():
        (outdir / (name + ".log")).write_text(text, encoding="utf-8",
                                              errors="replace")
    print(f"\nraw logs -> {outdir}")
    if fails:
        print(f"install_mismatch_probe: {len(fails)} FAILURE(S): "
              + ", ".join(fails))
        return 1
    print("install_mismatch_probe: all arms passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
