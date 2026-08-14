"""Smoke test for tools/tubuild.py, run against the real committed pilot artifacts.

Per the assignment this tool generalizes (notes/tu-reconstruction-pilot-report.md):
if `tubuild.py verify` disagrees with pilot #1's hand-verified result for
ov045/PoleLift (7/7 MATCH, clean objisolate, one known destructor-order anomaly),
the TOOL has a bug, not the pilot. This file is that check, automated.

Runs the real CLI via subprocess (not the internal functions) so it exercises
exactly what a human would type. Skips itself when the pinned compiler or the
extracted ROM is not present, the same guard tools/test_build_pin.py uses.

Usage:
    python tools/test_tubuild.py
"""
import pathlib
import shutil
import subprocess
import sys
import tempfile

TOOLS = pathlib.Path(__file__).resolve().parent
REPO = TOOLS.parent
sys.path.insert(0, str(TOOLS))

import build_pin as BP     # noqa: E402
import match as M          # noqa: E402


def _toolchain():
    return (M.MW / BP.DEFAULT_VERSION / "mwccarm.exe").is_file() and M.ARM9.is_file()


def _run(*args, manifest=None, timeout=180):
    # `verify`/`create` write their result back into the manifest they're given.
    # Tests that exercise those must pass `manifest=` (a scratch copy) so the
    # real, tracked config/tu_manifest.json is never mutated by running the test
    # suite -- `--manifest` is a top-level flag, so it has to precede the
    # subcommand on the argv line, not follow it.
    prefix = ["--manifest", str(manifest)] if manifest is not None else []
    r = subprocess.run([sys.executable, str(TOOLS / "tubuild.py"), *prefix, *args],
                       capture_output=True, text=True, cwd=REPO, timeout=timeout)
    return r.returncode, r.stdout + r.stderr


def _scratch_manifest():
    """A private copy of the real manifest, seeded with today's entries, that a
    write-back test can mutate freely. Caller is responsible for cleanup.

    mkstemp's fd is closed immediately -- Windows refuses to unlink a file that
    still has an open handle, and the child `tubuild.py` process (not this one)
    is what actually needs to read/write the path."""
    fd, path = tempfile.mkstemp(suffix=".json", prefix="tu_manifest_scratch_")
    import os
    os.close(fd)
    tmp = pathlib.Path(path)
    shutil.copyfile(REPO / "config" / "tu_manifest.json", tmp)
    return tmp


# --------------------------------------------------------------------------- list

def test_list_finds_polelift_and_its_module_neighbours():
    """No compiler needed: `list` only reads build/tu_map.json, config/, and src/."""
    code, out = _run("list", "--module", "ov045", "--verify-sample", "0")
    assert code == 0, out
    assert "ov045/PoleLift" in out
    assert "ov045/FireSeaElevator" in out          # its lower-address neighbour
    assert "ov045/ExtendingPlatform" in out         # its higher-address neighbour
    # PoleLift is 7 functions, all `complete` -- see the manifest and the pilot report.
    assert "7     7/7" in out


def test_list_has_no_duplicate_ids_anywhere_in_the_tree():
    """Regression guard: build/tu_map.json can legitimately emit two disjoint runs
    with the same class label (plan sec 2's "a class can span several TUs", real
    for ov006's dScMgBSC_c and eleven siblings) and, separately, two distinct
    unattributed runs that start at the same address (observed for itcm). Both
    used to collide into one tu_id before module_unit_ids() disambiguated them."""
    code, out = _run("list", "--verify-sample", "0")
    assert code == 0, out
    lines = [ln for ln in out.splitlines() if ln.startswith(("ov", "arm9/", "itcm/", "dtcm/"))]
    ids = [ln.split()[0] for ln in lines]
    assert len(ids) > 400, "suspiciously few candidate TUs parsed out of `list`'s output"
    dupes = {i for i in ids if ids.count(i) > 1}
    assert not dupes, f"duplicate tu_id(s) in `list` output: {sorted(dupes)[:5]}"


# ------------------------------------------------------------------------ inspect

def test_inspect_polelift_reproduces_the_pilots_static_findings():
    """No compiler strictly needed for these fields, but build_pin.verify also runs
    inside `inspect`, so this is skipped without the toolchain like the rest."""
    if not _toolchain():
        return
    code, out = _run("inspect", "ov045/PoleLift")
    assert code == 0, out
    assert "classes           PoleLift" in out
    assert "boundary conf.    high" in out
    # The pilot's report sec 4: D1/D0 are real, D2 is a compiler-only byproduct
    # that only appears once the TU is actually compiled (verify/compile), not here.
    assert "_ZN8PoleLiftD1Ev" in out and "_ZN8PoleLiftD0Ev" in out
    assert "D0/D1/D2 destructor variants     : ['_ZN8PoleLiftD1Ev', '_ZN8PoleLiftD0Ev']" in out
    # The pilot report sec 4: this TU emits the class's vtable because it defines
    # the key function (~PoleLift is the only virtual PoleLift declares itself).
    assert "_ZTV8PoleLift" in out
    assert "all functions verify under the build's pin: True" in out
    assert "manifest entry: status=" in out


# ------------------------------------------------------------------- compile+verify

def test_verify_reproduces_pilot_1s_7_of_7_and_clean_objisolate():
    """The headline check per the assignment: does tubuild.py verify's automated
    walk of match.py + objisolate.py + reloc_audit.py agree with what a human
    found by hand for this exact TU (notes/tu-reconstruction-pilot-report.md sec 1)?

    Runs against the ALREADY-CURATED src_tu/actors/PoleLift.cpp -- `create` is not
    involved, matching the assignment's instruction to skip it for this candidate.
    """
    if not _toolchain():
        return
    shadow = REPO / "src_tu" / "actors" / "PoleLift.cpp"
    assert shadow.is_file(), "the pilot's committed shadow TU is missing"

    scratch = _scratch_manifest()
    try:
        code, out = _run("verify", "ov045/PoleLift", manifest=scratch)
    finally:
        scratch.unlink(missing_ok=True)
    assert code == 0, out

    # Every one of the pilot's seven MATCH lines, independently reproduced.
    for sym, addr, size in [
            ("_ZN8PoleLiftD1Ev", "0x0211150c", "0x04c"),
            ("_ZN8PoleLiftD0Ev", "0x02111558", "0x060"),
            ("_ZN8PoleLift16CleanupResourcesEv", "0x021115b8", "0x038"),
            ("_ZN8PoleLift6RenderEv", "0x021115f0", "0x028"),
            ("_ZN8PoleLift8BehaviorEv", "0x02111618", "0x120"),
            ("_ZN8PoleLift13InitResourcesEv", "0x02111738", "0x0d0"),
            ("PoleLift_Spawn", "0x02111808", "0x038")]:
        assert f"MATCH    {sym}" in out and addr in out, (sym, out)

    assert "byte comparison   : 7/7 MATCH" in out
    # This is the check pilot #1 needed and match.compare alone could not give:
    # the vtable-addend bug (PoleLift_Spawn's `+2` fix) would show here as a
    # failure even though the byte compare above is a false-green 7/7.
    assert "objisolate check  : clean" in out
    # The one documented, non-fixable anomaly (pilot report sec 3): the compiler
    # emits D2/D0/D1 in a fixed order that puts D0 before D1, opposite the ROM.
    assert "1 ordinal pair(s) NOT in ROM order: [(0, 1)]" in out
    assert "Result: 7/7 MATCH, objisolate clean, reloc-destinations clean -> TEXT-VERIFIED" in out
    # 3 unlicensed .text (D2 + Platform's two out-of-line vague-linkage dtors) +
    # 12 unlicensed .data (the vtable + RTTI records) = 15, exactly the pilot's
    # sec 4 inventory -- present and correctly refusing promotion, not silently
    # dropped.
    assert "15 unlicensed section/symbol(s) present -> PROMOTION REFUSED" in out
    assert "_ZN8PoleLiftD2Ev" in out and "_ZTV8PoleLift" in out


def test_compile_report_matches_the_pilots_object_inventory():
    """43 sections, 10 .text, 12 .data -- notes/tu-reconstruction-pilot-report.md
    sec 4, reproduced independently by tubuild.py's own ELF walk."""
    if not _toolchain():
        return
    code, out = _run("compile", "ov045/PoleLift")
    assert code == 0, out
    assert "sections (43):" in out
    # Anchored to the section-LISTING line shape ("[ NN] .text  type=SHT_..."), not
    # a bare substring: "-> section[N] .text  size=..." in the function-mapping
    # block below it also contains the text "] .text ", which a plain count()
    # over-matched by 7 (one per manifest function) the first time this ran.
    import re
    n_text = len(re.findall(r"\[\s*\d+\] \.text\s+type=SHT_", out))
    n_data = len(re.findall(r"\[\s*\d+\] \.data\s+type=SHT_", out))
    assert n_text == 10, f"expected 10 .text sections, counted {n_text}"
    assert n_data == 12, f"expected 12 .data sections, counted {n_data}"
    assert "UNLICENSED function symbols (3)" in out
    assert "UNLICENSED object/data symbols (12)" in out
    assert (REPO / "build" / "tu" / "ov045-PoleLift" / "inventory.txt").is_file()
    assert (REPO / "build" / "tu" / "ov045-PoleLift" / "PoleLift.o").is_file()


# ---------------------------------------------------------------- partial isolation

def test_partial_reproduces_the_production_per_function_objects():
    """plan sec 9 / phase D, as a check.

    The claim under test is the one that lets a consolidated source become canonical
    without whole-range linking: compile the TU ONCE, reduce it once per licensed
    function, and each derived object is what the current per-function
    compile+isolate pipeline already produces. A DIFFERS line here means the
    consolidated source is no longer a drop-in for the seven objects the ROM build
    makes today, whatever the byte comparison in `verify` says.

    `--no-record` so the tracked manifest is not rewritten by running the suite.
    """
    if not _toolchain():
        return
    code, out = _run("partial", "ov045/PoleLift", "--no-record", timeout=600)
    assert code == 0, out
    for sym in ("_ZN8PoleLiftD1Ev", "_ZN8PoleLiftD0Ev",
                "_ZN8PoleLift16CleanupResourcesEv", "_ZN8PoleLift6RenderEv",
                "_ZN8PoleLift8BehaviorEv", "_ZN8PoleLift13InitResourcesEv",
                "PoleLift_Spawn"):
        line = next((l for l in out.splitlines() if f" {sym} " in l), None)
        assert line and "IDENTICAL" in line, (sym, line, out)
    assert "contribution equivalence: 7/7" in out
    assert "7/7 CONTRIBUTION-EQUIVALENT" in out
    # The one class of difference partial isolation DOES introduce: extra imports, from
    # one merged symbol table serving all seven reductions. They are inert only while
    # something in the link defines them; a name with no home at all is a real risk and
    # must stay empty for this TU.
    assert ("imports with no symbols.txt home anywhere AND not already imported by the "
            "objects being replaced: none") in out


# ------------------------------------------------------------------------------ CLI

def test_promote_refuses_to_mutate_without_dry_run():
    """The mutating half of `promote` deletes enrolled src/ files and edits tracked
    delinks.txt. Until that lands it must refuse loudly, not half-execute."""
    code, out = _run("promote", "ov045/PoleLift")
    assert code != 0
    assert "only --dry-run is implemented" in out


def test_promote_dry_run_refuses_a_tu_that_is_not_link_verified_but_still_explains():
    """plan sec 7.7: promotion is refused unless every required gate is green -- and
    the dry run still has to PRINT the plan, because seeing what a promotion would do
    is the point of a dry run even when it would be refused."""
    code, out = _run("promote", "ov045/PoleLift", "--dry-run")
    assert code != 0, "text-verified is not enough to promote"
    assert "promotion would be REFUSED" in out
    assert "git mv src_tu/actors/PoleLift.cpp" in out
    assert "git rm src/_ZN8PoleLift6RenderEv.cpp" in out
    assert "NOTHING IS WRITTEN BY THIS COMMAND" in out
    assert "DRY RUN COMPLETE" in out
    assert (REPO / "src_tu" / "actors" / "PoleLift.cpp").is_file()
    assert (REPO / "src" / "_ZN8PoleLift6RenderEv.cpp").is_file()


def test_linkcheck_refuses_a_tu_whose_legacy_entries_are_not_complete():
    """ov045/FallBlockBfs is text-verified but NONE of its five delinks entries carries
    `complete`, so dsd serves that whole range from ROM bytes today. Substituting a TU
    there would enroll five ranges and consolidate them in one step, and a green result
    would not say which change earned it. This must refuse before it links -- and it is
    the cheap half of linkcheck, so it is the part a test suite can afford to run."""
    code, out = _run("linkcheck", "ov045/FallBlockBfs", timeout=600)
    assert code != 0
    assert "REFUSED" in out
    assert "NOT `complete` today" in out


def test_splice_refuses_a_span_it_cannot_tile_exactly():
    """The substitution is silent when wrong: dsd fills any range it has no object for
    with retail ROM bytes, so an off-by-one span still links and still compares green
    while contributing nothing. Checked on a scratch COPY of a real delinks.txt."""
    import json
    sys.path.insert(0, str(TOOLS))
    import tubuild as T

    manifest = json.loads((REPO / "config" / "tu_manifest.json").read_text(encoding="utf-8"))
    entry = next(e for e in manifest["entries"] if e["id"] == "ov002/LevelObjects")
    sec = next(s for s in entry["sections"] if s["name"] == ".text")
    start, end = int(sec["start"], 16), int(sec["end"], 16)
    legacy = [f["legacy_source"] for f in entry["functions"]]
    real = REPO / "config" / "arm9" / "overlays" / "ov002" / "delinks.txt"

    with tempfile.TemporaryDirectory() as td:
        good = pathlib.Path(td) / "good.txt"
        shutil.copyfile(real, good)
        replaced, reasons = T.splice_tu_entry(good, start, end, entry["source"], legacy)
        assert reasons == [], reasons
        assert len(replaced) == len(legacy)
        assert f"{entry['source']}:" in good.read_text(encoding="utf-8")

        # End four bytes short: the last function is now half in, half out.
        short = pathlib.Path(td) / "short.txt"
        shutil.copyfile(real, short)
        replaced, reasons = T.splice_tu_entry(short, start, end - 4, entry["source"], legacy)
        assert replaced is None and reasons
        assert short.read_bytes() == real.read_bytes(), "a refusal must not edit the file"

        # Right span, but the manifest claims a function the span does not hold.
        wrong = pathlib.Path(td) / "wrong.txt"
        shutil.copyfile(real, wrong)
        replaced, reasons = T.splice_tu_entry(wrong, start, end, entry["source"],
                                              legacy + ["src/not_in_this_tu.cpp"])
        assert replaced is None
        assert any("not a delinks entry inside the span" in r for r in reasons), reasons
        assert wrong.read_bytes() == real.read_bytes()


def test_unknown_id_fails_closed_with_a_clear_reason():
    code, out = _run("inspect", "ov999/NoSuchClass")
    assert code != 0
    assert "no candidate TU" in out


if __name__ == "__main__":
    fails = 0
    ran = 0
    for nm, fn in sorted(globals().items()):
        if nm.startswith("test_") and callable(fn):
            ran += 1
            try:
                fn()
                print(f"  PASS  {nm}")
            except AssertionError as e:
                fails += 1
                print(f"  FAIL  {nm}: {e}")
    if not _toolchain():
        print("\n(NOTE: pinned compiler/ROM not found -- toolchain-dependent tests skipped "
             "themselves rather than failing)")
    print(f"\n{ran} test(s) run, {fails} failure(s)")
    sys.exit(1 if fails else 0)
