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


# ------------------------------------------------------------------------------ CLI

def test_linkcheck_and_promote_are_clearly_stubbed_not_silently_missing():
    for cmd in ("linkcheck", "promote"):
        code, out = _run(cmd, "ov045/PoleLift")
        assert code != 0
        assert "NOT YET IMPLEMENTED" in out


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
