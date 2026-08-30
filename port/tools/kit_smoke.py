#!/usr/bin/env python3
"""Run the SHIPPED kit the way a player runs it, on a machine that has no repo.

    python port/tools/kit_smoke.py <kit-dir> <rom.nds> [--work <dir>] [--keep]

THE RULE THIS TOOL EXISTS TO ENFORCE:

    THE KIT IS ONLY TESTED WHEN THE SHIPPED ARTIFACT RUNS WITHOUT THE REPO.

Everything else that was ever called "testing the kit" was a developer running
the packaged exe out of, or next to, a checkout that already had a complete
build/assets in it. That is not a test of the kit. It is a test of the
developer's machine, and it passed for weeks while every player extraction was
missing three files the game will not start without. port/kit_pipeline.txt is
the write-up; this is the check that would have caught it.

WHAT A CLEAN ROOM MEANS HERE, AND WHAT IT HONESTLY DOES NOT

The staged copy is a fresh directory holding only the kit's own files, run with
CWD set to it and SM64DS_ASSET_ROOT pointing at it, which is exactly what
play.bat and the launcher's GameRunner.Launch do. The repo is not deleted from
the machine -- no test should need that -- so "cannot see the repo" is proved
the way it can actually be proved:

  * the packaged exe is required to be a PORT_ROM_CLEAN build, and that build
    has no PORT_REPO_ROOT fallback (hal/asset_root_refuse.cpp). --prove-refusal
    runs the same exe with the environment stripped and requires it to refuse
    by name rather than quietly reading a checkout;
  * the staged directory is a copy, so nothing in it is a link back to a tree
    with a catalog in it;
  * the run is filesystem-audited: with SM64DS_NFS_TRACE unset there is nothing
    to intercept, so instead the check is structural -- if the exe had opened a
    repo path it would have had to get it from PORT_REPO_ROOT, and step 1 shows
    that path is refused, not followed.

WHAT IT CHECKS, IN ORDER, STOPPING AT THE FIRST REFUSAL

  1. the kit directory holds a plausible kit: an exe, play.bat,
     extract_assets.ps1, the romdata recipe and manifest.
  2. the exe is a PORT_ROM_CLEAN build. Same test package_kit.ps1 makes, for
     the same reason: a kit built without it embeds ROM bytes and must not
     ship, and it is also the build whose asset-root fallback is removed.
  3. MANIFEST AGREEMENT, both directions, against the SHIPPED BINARY rather
     than against the source:
       - every "build/assets/<name>" string in the exe is in
         port/kit_assets.txt, so a newly required file cannot be added without
         landing on the list;
       - every name in port/kit_assets.txt appears as a string in the exe, so a
         list entry cannot go stale.
     This is the drift check. The bug this lane fixed was precisely a new
     required file that the extractor never learned about, and reading the
     shipped exe is the only reading that could not have been fooled.
  4. stage a clean copy of the kit into a fresh temp directory and drop the
     ROM into its drop folder. Nothing else goes in.
  5. run the kit's own extractor there, exactly as play.bat runs it.
  6. every path in port/kit_assets.txt now exists in the staged directory.
     This is the writer/reader loop closing: what the exe demands (3) is what
     the extractor produced (6).
  7. run the exe the way GameRunner.Launch does -- CWD = bundle,
     SM64DS_ASSET_ROOT = bundle, SM64DS_VOLUME set, and nothing else -- as a
     counted headless selftest, and require a selftest BMP and a zero exit.
     This is the run that leaves evidence behind.
  7b. --live N (8 seconds by default, 0 skips): the SAME launch with nothing
     added at all, so the game opens its window and runs. THIS FLASHES A REAL
     WINDOW. It is the only step that is bit-for-bit a player's launch, and it
     is here because the bug this lane fixed killed the process in about 180 ms
     -- before any window existed. "Still alive after N seconds" is the shape
     of what the player actually saw; a counted headless run is not.
  8. --prove-refusal (on by default): run the same exe in the same directory
     with SM64DS_ASSET_ROOT REMOVED and require it to refuse loudly: non-zero
     exit, the missing file named on stderr, startup_error.txt written. A
     build that starts anyway is reading something it must not be able to see.

Exit 0 all green, 1 at the first red, with a one-line verdict per step.

THE BMP IS A LIVENESS CHECK HERE, NOT A RASTER COMPARISON. Its pixels track the
hosted-global layout and the .dsstate base (port/tools/battery.py says this at
length), and a kit exe is a different link from the gated one, so its BMP will
not match the battery's and is not compared against anything. What is being
asked is only "did this reach the renderer and write a frame".
"""
from __future__ import annotations

import argparse
import os
import pathlib
import re
import shutil
import subprocess
import sys
import tempfile

REPO = pathlib.Path(__file__).resolve().parent.parent.parent
MANIFEST = REPO / "port" / "kit_assets.txt"

# The marker package_kit.ps1 uses for the same question, kept spelled the same
# way on purpose: two checks that disagree about what "ROM-clean" looks like
# are worse than one.
ROM_CLEAN_MARK = b"build/assets/romdata.bin"

# What a kit directory has to hold before any of this is worth trying. Two
# shapes exist and each has its own entry point: the bare kit's play.bat, and
# the launcher bundle's SM64DSLauncher.exe (assemble_bundle.ps1's flat zip,
# which ships version.json and launcher_config.json beside it and no play.bat
# at all). Which shape this kit is decides which list applies and whether the
# play.bat freshness check below has a file to read.
KIT_REQUIRED = ("play.bat", "extract_assets.ps1", "romdata.recipe.tsv",
                "romdata.manifest")
KIT_REQUIRED_LAUNCHER = ("SM64DSLauncher.exe", "extract_assets.ps1",
                         "romdata.recipe.tsv", "romdata.manifest",
                         "version.json", "launcher_config.json")

ASSET_STRING = re.compile(rb"build/assets/([A-Za-z0-9_.\-]+)")

FAILED = []


def say(ok: bool, text: str) -> bool:
    print(("  ok   " if ok else "  FAIL ") + text)
    if not ok:
        FAILED.append(text)
    return ok


def die(text: str):
    print("  FAIL " + text)
    print("\nkit_smoke: RED")
    sys.exit(1)


def read_manifest() -> list[str]:
    if not MANIFEST.is_file():
        die(f"no manifest at {MANIFEST}")
    out = []
    for line in MANIFEST.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if line and not line.startswith("#"):
            out.append(line)
    if not out:
        die(f"{MANIFEST} lists nothing")
    return out


def find_exe(kit: pathlib.Path) -> pathlib.Path:
    """The GAME exe. The launcher-era bundle legitimately ships two exes --
    SM64DSLauncher.exe beside the game -- and the launcher is not the thing
    this smoke runs: it is a .NET UI that would sit waiting for a person.
    So the launcher is excused BY ITS EXACT NAME and nothing else is; a third
    exe of any name still dies, because a stray exe in a kit is still the
    defect this check exists to catch."""
    exes = sorted(p for p in kit.glob("*.exe")
                  if p.name != "SM64DSLauncher.exe")
    if len(exes) != 1:
        die(f"expected exactly one game .exe in {kit}, found {len(exes)}")
    return exes[0]


def check_manifest_against_exe(exe: pathlib.Path, wanted: list[str]) -> None:
    """Both directions, read off the shipped binary."""
    blob = exe.read_bytes()
    in_exe = {m.decode() for m in ASSET_STRING.findall(blob)}
    listed_assets = {p.split("/")[-1] for p in wanted
                     if p.startswith("build/assets/")}
    # EXACTLY TWO NAMES GET THE BARE-STRING TREATMENT, and the narrowness is the
    # point. hal/fs_names.cpp's slurp() is the only seam that opens through a
    # "%s/build/assets/%s" format with the name passed as a separate argument,
    # and these are the only two names it passes, so they are the only entries
    # that legitimately appear in the exe without "build/assets/" in front.
    #
    # Widening this to every listed basename (an earlier draft did) weakens the
    # STALE direction and nothing else: a bare "romdata.bin" occurring anywhere
    # in the image for any reason would vouch for a manifest entry the exe had
    # stopped wanting. The NOT-LISTED direction never used this set, so it is
    # unaffected either way.
    for name in ("nitrofs_fnt.bin", "nitrofs_fat.bin"):
        if name in listed_assets and name.encode() in blob:
            in_exe.add(name)

    unlisted = sorted(n for n in in_exe
                      if f"build/assets/{n}" not in wanted)
    say(not unlisted,
        "every build/assets string in the exe is in kit_assets.txt"
        + ("" if not unlisted else f" -- NOT LISTED: {', '.join(unlisted)}"))

    stale = sorted(n for n in listed_assets if n not in in_exe)
    say(not stale,
        "every kit_assets.txt build/assets entry is wanted by the exe"
        + ("" if not stale else f" -- STALE: {', '.join(stale)}"))


PLAY_BAT_TEST = re.compile(r'if not exist "%KIT%\\(?P<path>[^"]+)"\s+goto unpack',
                           re.IGNORECASE)


def check_play_bat(kit: pathlib.Path, wanted: list[str]) -> None:
    """play.bat decides whether to extract. Its list must be the manifest.

    THIS IS THE CHECK THAT WOULD HAVE SAVED THE EXISTING INSTALLS. The shipped
    play.bat tested three files, all three of which an OUT-OF-DATE extraction
    already had, so it said "already unpacked" about an install that was missing
    everything added since. A fixed extractor does not help a player whose
    play.bat never calls it. Both directions, because a list that tests too much
    costs one re-extraction and a list that tests too little costs a player a
    game that cannot repair itself.
    """
    text = (kit / "play.bat").read_text(encoding="utf-8", errors="replace")
    tested = {m.group("path").replace("\\", "/")
              for m in PLAY_BAT_TEST.finditer(text)}
    untested = sorted(p for p in wanted if p not in tested)
    say(not untested,
        "play.bat's unpack test names every required file"
        + ("" if not untested else f" -- NOT TESTED: {', '.join(untested)}"))
    extra = sorted(p for p in tested if p not in wanted)
    say(not extra,
        "play.bat tests nothing that is not on the list"
        + ("" if not extra else f" -- UNKNOWN: {', '.join(extra)}"))


def stage(kit: pathlib.Path, rom: pathlib.Path, work: pathlib.Path) -> pathlib.Path:
    """A fresh directory holding the kit's files and the player's ROM. Nothing
    else. A copy, never a link: a junction back into a tree with a catalog in
    it would reintroduce the exact mask this whole lane is about."""
    # Refuse a kit that has already been run in place, BEFORE copying it. Such a
    # directory holds a whole extracted cartridge, so staging it would copy
    # gigabytes and then test an extraction this run did not perform -- which is
    # the same "it worked on a machine that already had the data" mistake in
    # miniature.
    #
    # "Run in place" is detected by EXTRACTION PRODUCTS, not by the bare
    # existence of build/: the launcher bundle SHIPS build/assets/
    # romdata.manifest (BUNDLE.md's contract -- the exe reads its assets
    # through that path), so a fresh bundle owns a build directory the moment
    # it is unzipped. What a fresh kit can never own is a file the extractor
    # makes from ROM bytes.
    for stale in ("extracted", "build/assets/romdata.bin",
                  "build/assets/nitrofs.tsv"):
        if (kit / stale).exists():
            die(f"{kit / stale} exists: this kit has been run in place, so it is "
                f"not a clean kit. Package a fresh one with -Output <a new dir>.")
    bundle = work / "bundle"
    if bundle.exists():
        shutil.rmtree(bundle)
    bundle.mkdir(parents=True)
    for item in kit.iterdir():
        if item.is_dir():
            shutil.copytree(item, bundle / item.name)
        else:
            shutil.copy2(item, bundle / item.name)
    # Whatever the drop folder is called in this kit, use it; make the current
    # name if the kit shipped neither.
    drop = None
    for name in ("PLACE YOUR ROM HERE", "PLACE EU ROM HERE"):
        if (bundle / name).is_dir():
            drop = bundle / name
            break
    if drop is None:
        drop = bundle / "PLACE YOUR ROM HERE"
        drop.mkdir()
    shutil.copy2(rom, drop / rom.name)
    return bundle


def run_extractor(bundle: pathlib.Path) -> int:
    """Exactly play.bat's invocation: the script read as TEXT and run as a
    script block, so a machine with a stricter execution policy behaves the
    same here as it does for a player."""
    script = bundle / "extract_assets.ps1"
    cmd = ("$ErrorActionPreference='Stop'; "
           "& ([scriptblock]::Create([IO.File]::ReadAllText("
           f"'{script}'))) -Destination '{bundle}'")
    p = subprocess.run(
        ["powershell", "-NoProfile", "-NonInteractive",
         "-ExecutionPolicy", "Bypass", "-OutputFormat", "Text", "-Command", cmd],
        cwd=str(bundle), capture_output=True, text=True, errors="replace")
    tail = [ln for ln in (p.stdout or "").splitlines()
            if not re.match(r"^\s+\d+ of \d+$", ln)]
    for line in tail[-14:]:
        print("       | " + line)
    if p.returncode != 0 and p.stderr:
        for line in p.stderr.splitlines()[-8:]:
            print("       ! " + line)
    return p.returncode


def launch_env(bundle: pathlib.Path, *, asset_root: bool) -> dict:
    """The launcher's environment, and only it.

    SM64DSLauncher's GameRunner.Launch sets WorkingDirectory to the bundle and
    puts SM64DS_ASSET_ROOT and SM64DS_VOLUME in the child's environment. Every
    other SM64DS_* knob is popped, because an inherited variable from the shell
    that started this script would be testing something no player has.
    """
    env = {k: v for k, v in os.environ.items() if not k.startswith("SM64DS_")}
    if asset_root:
        env["SM64DS_ASSET_ROOT"] = str(bundle)
    env["SM64DS_VOLUME"] = "50"
    return env


def run_game(exe: pathlib.Path, bundle: pathlib.Path, frames: int) -> tuple[int, str, str]:
    """Headless, counted, deterministic: the run that leaves evidence behind."""
    env = launch_env(bundle, asset_root=True)
    env["SM64DS_WINDOW_SELFTEST"] = str(frames)
    env["SM64DS_NO_DIALOG"] = "1"
    p = subprocess.run([str(exe)], cwd=str(bundle), env=env,
                       capture_output=True, text=True, errors="replace",
                       timeout=600)
    return p.returncode, p.stdout or "", p.stderr or ""


def run_live(exe: pathlib.Path, bundle: pathlib.Path, seconds: int) -> None:
    """A REAL LAUNCH, and the only step here that is bit-for-bit what a player
    gets: GameRunner.Launch's environment with nothing added, so the game opens
    its window and runs until something stops it.

    THIS OPENS A VISIBLE WINDOW for a few seconds. That is the point. The bug
    this whole lane is about killed the process in roughly 180 ms, before any
    window existed, and no counted headless run reproduces the shape of what
    the player saw. "Still alive after N seconds" does, exactly.

    The selftest run above is the run that leaves evidence; this is the run
    that is honest about the entry point. Neither replaces the other."""
    env = launch_env(bundle, asset_root=True)
    env["SM64DS_NO_DIALOG"] = "1"
    p = subprocess.Popen([str(exe)], cwd=str(bundle), env=env,
                         stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        rc = p.wait(timeout=seconds)
        say(False, f"the game died after less than {seconds}s (exit {rc}) -- "
                   "a player would see this as 'it does not start'")
        err = bundle / "startup_error.txt"
        if err.is_file():
            for line in err.read_text(errors="replace").splitlines():
                print("       > " + line)
        logs = sorted((bundle / "playlog").glob("*.log")) if (bundle / "playlog").is_dir() else []
        if logs:
            for line in logs[-1].read_text(errors="replace").splitlines()[-12:]:
                print("       ! " + line)
    except subprocess.TimeoutExpired:
        say(True, f"still running after {seconds}s (a real launch, window and all)")
        p.terminate()
        try:
            p.wait(timeout=20)
        except subprocess.TimeoutExpired:
            p.kill()


def prove_refusal(exe: pathlib.Path, bundle: pathlib.Path) -> None:
    """The same exe, the same folder, no asset root. It must refuse."""
    err_file = bundle / "startup_error.txt"
    if err_file.exists():
        err_file.unlink()
    env = launch_env(bundle, asset_root=False)
    env["SM64DS_NO_DIALOG"] = "1"
    p = subprocess.run([str(exe)], cwd=str(bundle), env=env,
                       capture_output=True, text=True, errors="replace",
                       timeout=300)
    err = (p.stderr or "") + (p.stdout or "")
    for line in err.splitlines()[:8]:
        print("       > " + line)
    # EXACTLY 2, not merely non-zero. hal/asset_root_refuse.cpp exits rather
    # than aborting, on purpose and at length: MSVC's abort() raises
    # STATUS_STACK_BUFFER_OVERRUN, the shell reports 0xC0000409, and every crash
    # reporter in the chain then files a configuration error as a memory-safety
    # crash in the port. A non-zero assertion passes happily on that 0xC0000409,
    # so it would not notice the design being undone. 2 is the code the other
    # refusals on this path use and the code a launcher can branch on.
    say(p.returncode == 2,
        f"a run with no SM64DS_ASSET_ROOT exits 2, not merely non-zero "
        f"(got {p.returncode})")
    say("SM64DS_ASSET_ROOT" in err and "FATAL" in err,
        "the refusal names the variable on stderr")
    say("build/assets/" in err or "extracted/" in err,
        "the refusal names the file it was about to open")
    say(err_file.is_file() and err_file.stat().st_size > 0,
        "startup_error.txt is written for the launcher to show")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("kit", type=pathlib.Path, help="a directory package_kit.ps1 produced")
    ap.add_argument("rom", type=pathlib.Path, help="a Super Mario 64 DS .nds dump")
    ap.add_argument("--work", type=pathlib.Path,
                    help="where to stage (default: a fresh temp directory)")
    ap.add_argument("--frames", type=int, default=300,
                    help="selftest frame count (default 300)")
    ap.add_argument("--live", type=int, default=8, metavar="SECONDS",
                    help="also do a real windowed launch and require it to "
                         "survive this long (default 8; 0 skips). THIS OPENS A "
                         "VISIBLE WINDOW -- it is the only step that is exactly "
                         "what a player gets")
    ap.add_argument("--keep", action="store_true",
                    help="leave the staged directory behind for inspection")
    ap.add_argument("--no-prove-refusal", action="store_true",
                    help="skip the no-asset-root refusal check")
    args = ap.parse_args()

    kit = args.kit.resolve()
    rom = args.rom.resolve()
    if not kit.is_dir():
        die(f"no kit directory at {kit}")
    if not rom.is_file():
        die(f"no ROM at {rom}")

    wanted = read_manifest()

    print(f"kit    : {kit}")
    print(f"rom    : {rom}")
    print(f"assets : {MANIFEST} ({len(wanted)} required paths)")
    print()

    print("1. the kit directory")
    exe = find_exe(kit)
    say(True, f"exe: {exe.name} ({exe.stat().st_size:,} bytes)")
    launcher_shape = (kit / "SM64DSLauncher.exe").is_file()
    required = KIT_REQUIRED_LAUNCHER if launcher_shape else KIT_REQUIRED
    say(True, "shape: " + ("launcher bundle (assemble_bundle.ps1)"
                           if launcher_shape else "bare kit (package_kit.ps1)"))
    for name in required:
        if not say((kit / name).is_file(), f"ships {name}"):
            die("the kit is incomplete; its packaging script did not finish")

    print("\n2. the exe is a shipping (PORT_ROM_CLEAN) build")
    blob = exe.read_bytes()
    if not say(ROM_CLEAN_MARK in blob,
               "carries the ROM-CLEAN boot loader's path string"):
        die("this exe still embeds ROM data and must not ship")

    print("\n3. kit_assets.txt agrees with the shipped exe")
    check_manifest_against_exe(exe, wanted)

    if launcher_shape:
        # play.bat's unpack-freshness list lives in GameRunner.Launch in the
        # launcher bundle, compiled C# this text check cannot read. Said out
        # loud rather than silently skipped, so a RED-vs-GREEN diff between
        # the two shapes never hides which checks each one actually ran.
        print("\n3b. play.bat freshness list: not in this shape "
              "(the launcher's GameRunner owns it)")
    else:
        print("\n3b. kit_assets.txt agrees with the shipped play.bat")
        check_play_bat(kit, wanted)

    if FAILED:
        print("\nkit_smoke: RED")
        return 1

    work = args.work.resolve() if args.work else pathlib.Path(
        tempfile.mkdtemp(prefix="kitsmoke_"))
    work.mkdir(parents=True, exist_ok=True)
    print(f"\n4. staging a clean player machine at {work}")
    bundle = stage(kit, rom, work)
    say(True, f"bundle: {bundle}")
    # Same rule as stage()'s refusal: what a fresh bundle can never hold is a
    # file the extractor makes from ROM bytes. The launcher bundle legitimately
    # ships build/assets/romdata.manifest, so "no build/ at all" is the wrong
    # assertion for that shape.
    say(not (bundle / "build/assets/romdata.bin").exists()
        and not (bundle / "build/assets/nitrofs.tsv").exists(),
        "the staged bundle starts with no extracted ROM data of its own")

    print("\n5. running the kit's own extractor, as play.bat runs it")
    rc = run_extractor(bundle)
    if not say(rc == 0, f"extract_assets.ps1 exit {rc}"):
        print("\nkit_smoke: RED")
        return 1

    print("\n6. the extractor produced every path the exe requires")
    missing = [p for p in wanted if not (bundle / p).exists()]
    for p in wanted:
        f = bundle / p
        say(f.exists(), f"{p}" + (f"  ({f.stat().st_size:,} bytes)"
                                  if f.exists() else "  MISSING"))
    if missing:
        print("\nkit_smoke: RED -- this is the shape of the shipping bug: the "
              "exe wants a file the extractor never writes.")
        return 1

    print(f"\n7. the shipped entry point: {exe.name}, {args.frames} frames, "
          "launcher environment")
    bexe = bundle / exe.name
    rc, out, err = run_game(bexe, bundle, args.frames)
    for line in (out.splitlines()[-10:] if out else []):
        print("       | " + line)
    for line in (err.splitlines()[-10:] if err else []):
        print("       ! " + line)
    say(rc == 0, f"exit {rc}")
    bmps = sorted(bundle.glob("*selftest*.bmp"))
    say(bool(bmps), "a selftest BMP was written"
        + (f": {bmps[0].name} ({bmps[0].stat().st_size:,} bytes)" if bmps else ""))
    # AND IT REACHED THE TITLE, which is a new question and a stronger one.
    #
    # This arm launches with the launcher's environment and nothing else, so
    # since the owner's boot-to-title ruling it is a TITLE run. The two checks
    # above are unchanged and still say exactly what they always said -- the
    # exe survived N frames and reached the renderer -- but on their own they
    # would now be satisfied by an exe that booted anything at all, the old
    # level included, which is precisely the regression this ruling can suffer
    # and the one nothing else in the kit would catch.
    #
    # The line is hal/scene_boot.cpp's own boot report, printed by
    # port_scene_boot once the ROM's spawn spine has returned a scene object.
    # It is not a message this script asked the game to print; it is the game
    # saying which scene came up. Nothing above is relaxed to make room for it.
    both = (out or "") + (err or "")
    say("[scene] 1 = " in both,
        "the shipped exe booted the TITLE, not a level -- the game's own "
        "\"[scene] 1 = \" boot line is in the output")

    if args.live:
        print(f"\n7b. a real launch -- THIS OPENS A WINDOW for {args.live}s")
        run_live(bexe, bundle, args.live)

    if not args.no_prove_refusal:
        print("\n8. the same exe with no SM64DS_ASSET_ROOT must refuse")
        prove_refusal(bexe, bundle)

    if not args.keep and not args.work:
        shutil.rmtree(work, ignore_errors=True)
    else:
        print(f"\nstaged directory kept: {work}")

    print()
    if FAILED:
        print(f"kit_smoke: RED ({len(FAILED)} failed)")
        for t in FAILED:
            print("    " + t)
        return 1
    print("kit_smoke: GREEN")
    return 0


if __name__ == "__main__":
    sys.exit(main())
