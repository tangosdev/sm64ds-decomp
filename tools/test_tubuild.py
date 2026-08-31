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
import os
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
    # real, tracked config/tu_manifest.d/ is never mutated by running the test
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
    # A single .json is still a legal manifest root -- tu_manifest.save writes
    # one file when the root has a .json suffix -- so the scratch copy stays a
    # single file even though the tracked manifest is now a directory.
    sys.path.insert(0, str(TOOLS))
    import tu_manifest as TUM
    TUM.save(TUM.load(), tmp)
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
    # 1 unlicensed .text (D2) + 11 unlicensed .data (the vtable + the RTTI records)
    # = 12 -- present and correctly refusing promotion, not silently dropped.
    #
    # THE PILOT'S SEC 4 INVENTORY SAID 15, AND SO DID THIS LINE UNTIL IT WAS MEASURED.
    # The extra three were Platform's: two out-of-line vague-linkage destructors and
    # _ZTV8Platform, emitted here because Platform had no key function to anchor them.
    # #1555 ("Give Platform its 32nd vtable slot: Platform::Kill") gave it one, and
    # they moved to Platform's own TU. Compiling this file's own historical forms
    # against their own include/ trees reads 43 sections / 15 unlicensed at dedaa139e^
    # and 37 / 12 at dedaa139e -- so this expectation went stale at #1555, 116 pull
    # requests before the collision rename (#1643) that later stopped the file
    # compiling at all and hid the staleness behind a compile error.
    assert "12 unlicensed section/symbol(s) present -> PROMOTION REFUSED" in out
    assert "_ZN8PoleLiftD2Ev" in out and "_ZTV8PoleLift" in out


def test_compile_report_matches_the_pilots_object_inventory():
    """37 sections, 8 .text, 11 .data, reproduced independently by tubuild.py's own
    ELF walk.

    notes/tu-reconstruction-pilot-report.md sec 4 records 43/10/12, which is what this
    file emitted until #1555 anchored Platform's vtable and its two out-of-line
    destructors in Platform's own TU -- three fewer vague-linkage symbols here, and
    six fewer sections because each brings its own relocation section. The measurement
    is in test_verify_reproduces_pilot_1s_7_of_7_and_clean_objisolate above.
    """
    if not _toolchain():
        return
    code, out = _run("compile", "ov045/PoleLift")
    assert code == 0, out
    assert "sections (37):" in out
    # Anchored to the section-LISTING line shape ("[ NN] .text  type=SHT_..."), not
    # a bare substring: "-> section[N] .text  size=..." in the function-mapping
    # block below it also contains the text "] .text ", which a plain count()
    # over-matched by 7 (one per manifest function) the first time this ran.
    import re
    n_text = len(re.findall(r"\[\s*\d+\] \.text\s+type=SHT_", out))
    n_data = len(re.findall(r"\[\s*\d+\] \.data\s+type=SHT_", out))
    assert n_text == 8, f"expected 8 .text sections, counted {n_text}"
    assert n_data == 11, f"expected 11 .data sections, counted {n_data}"
    assert "UNLICENSED function symbols (1)" in out
    assert "UNLICENSED object/data symbols (11)" in out
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
    assert "COVERAGE GATE IS TU-AWARE" in out
    assert "function_snapshot() builds {stem: path}" not in out
    assert "tools/cpp_tu_compat.py" in out
    assert (REPO / "src_tu" / "actors" / "PoleLift.cpp").is_file()
    assert (REPO / "src" / "_ZN8PoleLift6RenderEv.cpp").is_file()


def test_splice_refuses_a_span_whose_legacy_entries_are_not_complete():
    """A delinks entry without `complete` is served from retail ROM bytes, so
    substituting a TU across one would enroll that range and consolidate it in the same
    step, and a green result would not say which change earned it. The substitution has
    to refuse before anything links.

    This used to run `linkcheck ov045/FallBlockBfs` and assert the refusal, because none
    of that TU's five entries carried `complete`. #1527 ("Phantom references: find the
    cause, clear two seams, +98 functions") marked all five, so the premise is gone: the
    command now gets past the splice and spends ninety seconds reaching a link that
    fails on the key-function wall instead. Re-pointing it at some other TU would only
    re-arm the same trap, so the invariant is checked directly, on a scratch COPY of the
    real file, the way its sibling test below does.
    """
    import json
    sys.path.insert(0, str(TOOLS))
    import tubuild as T

    manifest = T.load_manifest()
    entry = next(e for e in manifest["entries"] if e["id"] == "ov045/FallBlockBfs")
    sec = next(s for s in entry["sections"] if s["name"] == ".text")
    start, end = int(sec["start"], 16), int(sec["end"], 16)
    legacy = [f["legacy_source"] for f in entry["functions"]]
    real = REPO / "config" / "arm9" / "overlays" / "ov045" / "delinks.txt"

    with tempfile.TemporaryDirectory() as td:
        # As committed, every entry in the span is `complete`, so the splice is allowed.
        good = pathlib.Path(td) / "good.txt"
        shutil.copyfile(real, good)
        replaced, reasons = T.splice_tu_entry(good, start, end, entry["source"], legacy)
        assert reasons == [], reasons
        assert len(replaced) == len(legacy)

        # Take `complete` away from one of them and the splice must refuse, untouched.
        stripped = pathlib.Path(td) / "stripped.txt"
        text = real.read_text(encoding="utf-8")
        victim = legacy[0]
        head, sep, tail = text.partition(f"{victim}:\n")
        assert sep, victim
        stripped.write_text(head + sep + tail.replace("    complete\n", "", 1),
                            encoding="utf-8")
        before = stripped.read_bytes()
        replaced, reasons = T.splice_tu_entry(stripped, start, end, entry["source"], legacy)
        assert replaced is None
        assert any("NOT `complete` today" in r for r in reasons), reasons
        assert stripped.read_bytes() == before, "a refusal must not edit the file"


def test_splice_refuses_a_span_it_cannot_tile_exactly():
    """The substitution is silent when wrong: dsd fills any range it has no object for
    with retail ROM bytes, so an off-by-one span still links and still compares green
    while contributing nothing. Checked on a scratch COPY of a real delinks.txt."""
    import json
    sys.path.insert(0, str(TOOLS))
    import tubuild as T

    manifest = T.load_manifest()
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


def test_manifest_section_claims_are_explicit_and_unambiguous():
    claims, reasons = tubuild.manifest_section_claims({"sections": [
        {"name": ".text", "start": "0x1000", "end": "0x1010"},
        {"name": ".data", "start": "0x2000", "end": "0x2010"},
    ]})
    assert reasons == []
    assert claims == [
        {"name": ".text", "module_section": ".text",
         "start": 0x1000, "end": 0x1010},
        {"name": ".data", "module_section": ".data",
         "start": 0x2000, "end": 0x2010},
    ]

    _claims, reasons = tubuild.manifest_section_claims({"sections": [
        {"name": ".text", "start": "0x1000", "end": "0x1010"},
        {"name": ".data", "start": "0x2000", "end": "0x2010"},
        {"name": ".data", "start": "0x3000", "end": "0x3010"},
        {"name": ".mystery", "start": "0x4000", "end": "0x4010"},
    ]})
    assert any("duplicate .data claim" in r for r in reasons)
    assert any("unsupported name" in r for r in reasons)

    claims, reasons = tubuild.manifest_section_claims({"sections": [
        {"name": ".text", "start": "0x1000", "end": "0x1010"},
        {"name": ".rodata", "module_section": ".data",
         "start": "0x2040", "end": "0x2050"},
    ]})
    assert reasons == []
    assert claims[1] == {"name": ".rodata", "module_section": ".data",
                         "start": 0x2040, "end": 0x2050}

    _claims, reasons = tubuild.manifest_section_claims({"sections": [
        {"name": ".text", "start": "0x1000", "end": "0x1010"},
        {"name": ".rodata", "module_section": ".mystery",
         "start": "0x2040", "end": "0x2050"},
    ]})
    assert any("unsupported module_section" in r for r in reasons)


def test_splice_relinquishes_exact_manifest_data_and_bss_ranges_only():
    """The multi-section entry is written to a scratch copy; production config stays.

    The negative half plants an existing data owner and proves the same claim refuses
    without touching the file, rather than silently producing two owners.
    """
    text = (
        "    .text start:0x00001000 end:0x00001100 kind:code align:4\n"
        "    .data start:0x00002000 end:0x00002100 kind:data align:4\n"
        "    .bss start:0x00003000 end:0x00003100 kind:bss align:4\n"
        "src/one.c:\n"
        "    complete\n"
        "    .text start:0x00001000 end:0x00001010\n\n"
        "src/two.c:\n"
        "    complete\n"
        "    .text start:0x00001010 end:0x00001020\n"
    )
    claims = [
        {"name": ".text", "start": 0x1000, "end": 0x1020},
        {"name": ".data", "start": 0x2040, "end": 0x2050},
        {"name": ".bss", "start": 0x3080, "end": 0x3090},
    ]
    with tempfile.TemporaryDirectory() as td:
        path = pathlib.Path(td) / "delinks.txt"
        path.write_text(text, encoding="utf-8")
        replaced, reasons = tubuild.splice_tu_entry(
            path, 0x1000, 0x1020, "src_tu/T.cpp", ["src/one.c", "src/two.c"],
            section_claims=claims)
        assert reasons == []
        assert replaced == ["src/one.c", "src/two.c"]
        got = path.read_text(encoding="utf-8")
        assert got.count("src_tu/T.cpp:") == 1
        assert "    .text start:0x00001000 end:0x00001020" in got
        assert "    .data start:0x00002040 end:0x00002050" in got
        assert "    .bss start:0x00003080 end:0x00003090" in got
        assert "src/one.c:" not in got and "src/two.c:" not in got

        occupied = pathlib.Path(td) / "occupied.txt"
        occupied.write_text(text + "\nsrc/data.c:\n    complete\n"
                            "    .data start:0x00002048 end:0x00002058\n",
                            encoding="utf-8")
        before = occupied.read_bytes()
        replaced, reasons = tubuild.splice_tu_entry(
            occupied, 0x1000, 0x1020, "src_tu/T.cpp", ["src/one.c", "src/two.c"],
            section_claims=claims)
        assert replaced is None
        assert any("not pure gap ownership" in r for r in reasons)
        assert occupied.read_bytes() == before


def test_splice_maps_compiler_rodata_into_module_data():
    text = (
        "    .text start:0x00001000 end:0x00001100 kind:code align:4\n"
        "    .data start:0x00002000 end:0x00002100 kind:data align:4\n"
        "src/one.c:\n"
        "    complete\n"
        "    .text start:0x00001000 end:0x00001010\n"
    )
    claims = [
        {"name": ".text", "module_section": ".text",
         "start": 0x1000, "end": 0x1010},
        {"name": ".rodata", "module_section": ".data",
         "start": 0x2040, "end": 0x2050},
    ]
    with tempfile.TemporaryDirectory() as td:
        path = pathlib.Path(td) / "delinks.txt"
        path.write_text(text, encoding="utf-8")
        replaced, reasons = tubuild.splice_tu_entry(
            path, 0x1000, 0x1010, "src_tu/T.cpp", ["src/one.c"],
            section_claims=claims)
        assert reasons == []
        assert replaced == ["src/one.c"]
        got = path.read_text(encoding="utf-8")
        assert "    .rodata start:" not in got
        assert "    .data start:0x00002040 end:0x00002050" in got


def _compile_tu_fixture(source):
    with tempfile.TemporaryDirectory() as td:
        src = pathlib.Path(td) / "fixture.cpp"
        src.write_text("//cpp\n" + source, encoding="utf-8")
        flags = tubuild.RB.CFLAGS.replace("-lang c99", "-lang c++")
        return tubuild.M.compile_c(src, tubuild.RB.VERSION, flags)


def _vague_externalization_fixture():
    """Real mwcc vague RTTI plus injected canonical ROM/config evidence."""
    import io
    from elftools.elf.elffile import ELFFile
    from elftools.elf.relocation import RelocationSection

    obj = _compile_tu_fixture(
        "struct Base { virtual ~Base() {} };\n"
        "struct A : Base { virtual void a(); }; void A::a() {}\n")
    elf = ELFFile(io.BytesIO(obj))
    sections = list(elf.iter_sections())
    symtab = elf.get_section_by_name(".symtab")
    symbols = list(symtab.iter_symbols())
    addresses = {"_ZTI4Base": 0x5000, "_ZTS4Base": 0x5100}
    policies, targets, config, homes, name_index = [], {}, {"arm9": {}}, {}, {}
    next_destination = 0x6000
    for name, address in addresses.items():
        sym = next(s for s in symbols if s.name == name)
        section = sections[sym["st_shndx"]]
        relocs = []
        for relsec in sections:
            if not isinstance(relsec, RelocationSection) \
                    or relsec.header["sh_info"] != sym["st_shndx"]:
                continue
            for reloc in relsec.iter_relocations():
                target = symtab.get_symbol(reloc["r_info_sym"])
                relocs.append({
                    "offset": reloc["r_offset"], "type": "R_ARM_ABS32",
                    "kind": "load", "symbol": target.name,
                    "addend": reloc["r_addend"], "target_module": "arm9",
                    "target_address": next_destination,
                })
                config["arm9"][address + reloc["r_offset"]] = (
                    "load", next_destination, "arm9")
                raw_bias = (reloc["r_addend"] - tubuild.OI.VTABLE_PREAMBLE
                            if target.name.startswith("_ZTV")
                            and reloc["r_addend"] >= tubuild.OI.VTABLE_PREAMBLE
                            else reloc["r_addend"])
                name_index[target.name] = ("arm9", next_destination - raw_bias)
                next_destination += 4
        policies.append({
            "symbol": name, "disposition": "canonical-import",
            "section": ".data", "binding": "STB_LOPROC",
            "size": section.header["sh_size"], "canonical_module": "arm9",
            "canonical_address": address, "reason": "inherited RTTI fixture",
            "relocations": relocs,
        })
        targets[("arm9", address, section.header["sh_size"])] = section.data()
        homes[name] = [("arm9", address)]

    entry = {"module": "ov999", "functions": [], "data": [], "rodata": [],
             "bss": [], "externalized_output": policies}

    def target_reader(module, address, size):
        return targets.get((module, address, size))

    return obj, entry, homes, config, targets, target_reader, name_index


def test_nontext_verifier_checks_layout_bytes_symbols_and_reloc_destinations():
    if not _toolchain():
        return
    obj = _compile_tu_fixture(
        'extern "C" int external_value;\n'
        'extern "C" int *owned_ptr = &external_value;\n'
        'extern "C" int owned_word = 0x11223344;\n'
        'extern "C" int owned_zero = 0;\n')
    assert obj is not None
    data_start, bss_start = 0x2000, 0x3000
    data, error = tubuild.section_contribution(obj, ".data", data_start)
    assert error is None and data["relocs"]
    bss, error = tubuild.section_contribution(obj, ".bss", bss_start)
    assert error is None
    assert all(x == 0 for x in bss["bytes"])

    entry = {
        "module": "ov999", "functions": [],
        "data": [{"symbol": n, "address": f"0x{r['address']:08x}",
                  "size": f"0x{r['size']:x}"}
                 for n, r in data["symbols"].items() if r["type"] == "STT_OBJECT"],
        "bss": [{"symbol": n, "address": f"0x{r['address']:08x}",
                 "size": f"0x{r['size']:x}"}
                for n, r in bss["symbols"].items() if r["type"] == "STT_OBJECT"],
    }
    claims = [
        {"name": ".data", "start": data_start, "end": data_start + len(data["bytes"])},
        {"name": ".bss", "start": bss_start, "end": bss_start + len(bss["bytes"])},
    ]
    reloc = data["relocs"][0]
    source = data_start + reloc["offset"]
    destination = 0x12345678
    cfg = {"ov999": {source: ("load", destination, "arm9")}}
    entry["relocations"] = [{
        "section": ".data", "source": f"0x{source:08x}",
        "type": "R_ARM_ABS32", "kind": "load", "symbol": reloc["symbol"],
        "addend": reloc["addend"], "target_module": "arm9",
        "target_address": f"0x{destination:08x}",
    }]
    bss_homes = {row["symbol"]: [("ov999", int(row["address"], 0))]
                 for row in entry["bss"]}

    def target_reader(_module, start, size):
        assert start == data_start and size == len(data["bytes"])
        return data["bytes"]

    result = tubuild.verify_owned_sections(
        obj, entry, claims, name_index={"external_value": ("arm9", destination)},
        config_relocs=cfg, sym_index={}, target_reader=target_reader,
        symbol_homes=bss_homes, bss_boundaries={bss_start, bss_start + len(bss["bytes"])})
    assert result["ok"], result["errors"]
    assert result["relocations"][0]["verdict"] == "OK"

    wrong_module = tubuild.verify_owned_sections(
        obj, entry, claims, name_index={"external_value": ("ov123", destination)},
        config_relocs=cfg, sym_index={}, target_reader=target_reader,
        symbol_homes=bss_homes, bss_boundaries={bss_start, bss_start + len(bss["bytes"])})
    assert not wrong_module["ok"]
    assert any(r["verdict"] == "WRONG-MODULE" for r in wrong_module["relocations"])

    wrong_kind_entry = dict(entry)
    wrong_kind_entry["relocations"] = [dict(entry["relocations"][0], kind="arm_call")]
    wrong_kind = tubuild.verify_owned_sections(
        obj, wrong_kind_entry, claims,
        name_index={"external_value": ("arm9", destination)}, config_relocs=cfg,
        sym_index={}, target_reader=target_reader, symbol_homes=bss_homes,
        bss_boundaries={bss_start, bss_start + len(bss["bytes"])})
    assert not wrong_kind["ok"]
    assert any(r["verdict"] == "WRONG-KIND" for r in wrong_kind["relocations"])

    wrong_type_entry = dict(entry)
    wrong_type_entry["relocations"] = [dict(entry["relocations"][0], type="R_ARM_CALL")]
    wrong_type = tubuild.verify_owned_sections(
        obj, wrong_type_entry, claims,
        name_index={"external_value": ("arm9", destination)}, config_relocs=cfg,
        sym_index={}, target_reader=target_reader, symbol_homes=bss_homes,
        bss_boundaries={bss_start, bss_start + len(bss["bytes"])})
    assert not wrong_type["ok"]
    assert any(r["verdict"] == "WRONG-TYPE" for r in wrong_type["relocations"])

    wrong_addend_entry = dict(entry)
    wrong_addend_entry["relocations"] = [dict(entry["relocations"][0],
                                                  addend=reloc["addend"] + 4)]
    wrong_addend = tubuild.verify_owned_sections(
        obj, wrong_addend_entry, claims,
        name_index={"external_value": ("arm9", destination)}, config_relocs=cfg,
        sym_index={}, target_reader=target_reader, symbol_homes=bss_homes,
        bss_boundaries={bss_start, bss_start + len(bss["bytes"])})
    assert not wrong_addend["ok"]
    assert any(r["verdict"] == "WRONG-ADDEND" for r in wrong_addend["relocations"])

    # Wildcarding is limited to the relocated word. A different ordinary data word
    # must still fail the byte gate.
    damaged = bytearray(data["bytes"])
    protected = {reloc["offset"] & ~3}
    ordinary = next(i for i in range(0, len(damaged), 4) if i not in protected)
    damaged[ordinary] ^= 0xff
    bad_bytes = tubuild.verify_owned_sections(
        obj, entry, claims, name_index={"external_value": ("arm9", destination)},
        config_relocs=cfg, sym_index={},
        target_reader=lambda _m, _s, _n: bytes(damaged), symbol_homes=bss_homes,
        bss_boundaries={bss_start, bss_start + len(bss["bytes"])})
    assert not bad_bytes["ok"]
    assert any("non-relocated" in e for e in bad_bytes["errors"])


def test_compiler_only_policy_is_exact_and_refuses_a_real_rom_home():
    if not _toolchain():
        return
    obj = _compile_tu_fixture(
        "struct P { int p[4]; virtual ~P(); virtual void f(); };\nP::~P(){}\n")
    assert obj is not None
    base = {"functions": [{"symbol": "_ZN1PD0Ev"}, {"symbol": "_ZN1PD1Ev"}],
            "data": [], "bss": []}

    out, report, reasons = tubuild.apply_compiler_only_policy(obj, base, homes={})
    assert out is None
    assert any("_ZN1PD2Ev" in r and "no compiler_only_output" in r for r in reasons)

    entry = dict(base, compiler_only_output=[{
        "symbol": "_ZN1PD2Ev", "disposition": "deadstrip",
        "reason": "compiler-generated base-object destructor; no ROM symbol/caller",
    }])
    out, report, reasons = tubuild.apply_compiler_only_policy(obj, entry, homes={})
    assert reasons == [] and out is not None
    assert report["deadstripped"] == ["_ZN1PD2Ev"]

    out, _report, reasons = tubuild.apply_compiler_only_policy(
        obj, entry, homes={"_ZN1PD2Ev": [("ov999", 0x1234)]})
    assert out is None
    assert any("configured ROM home" in r for r in reasons)


def test_compiler_only_policy_reduces_data_before_its_duplicate_destructors():
    """A licensed vtable must not block removal of a duplicate dtor it names."""
    if not _toolchain():
        return
    obj = _compile_tu_fixture(
        "struct B { virtual ~B() {} };\n"
        "struct D : B { virtual ~D() {} virtual void f(); };\n"
        "void D::f() {}\n")
    assert obj is not None
    symbols = {s["name"]: s for s in tubuild.elf_inventory(obj)["symbols"]
               if s["name"] and not s["name"].startswith("$")}
    duplicates = sorted(n for n, s in symbols.items()
                        if s["type"] == "STT_FUNC" and n.startswith("_ZN1BD"))
    data = sorted(n for n, s in symbols.items()
                  if s["type"] == "STT_OBJECT"
                  and n.startswith(("_ZTV", "_ZTI", "_ZTS")))
    assert "_ZN1BD0Ev" in duplicates
    assert "_ZTV1B" in data

    # This is the old failure shape: the retained B vtable names B::~B(), so
    # removing only the function set is correctly refused by objisolate.
    _out, plan = tubuild.OI.derive_deadstrip(obj, duplicates)
    assert plan["error"] and "references compiler-only _ZN1BD" in plan["error"]

    licensed = sorted(n for n, s in symbols.items()
                      if s["type"] == "STT_FUNC" and n not in duplicates)
    policy = [
        {"symbol": n, "disposition": "deadstrip-duplicate",
         "reason": "vague base destructor with a canonical cartridge copy"}
        for n in duplicates
    ] + [
        {"symbol": n, "disposition": "deadstrip-data",
         "reason": "class data has a canonical cartridge copy"}
        for n in data
    ]
    homes = {n: [("arm9", 0x02010000 + i * 0x20)]
             for i, n in enumerate(duplicates + data)}
    entry = {"functions": [{"symbol": n} for n in licensed],
             "data": [], "bss": [], "compiler_only_output": policy}

    out, report, reasons = tubuild.apply_compiler_only_policy(obj, entry, homes=homes)
    assert reasons == [] and out is not None
    assert report["deadstripped"] == duplicates
    assert report["dataExternalized"] == data


def test_plain_deadstrip_is_refused_for_an_rtti_record():
    """The intact-object mirror of the rombuild guard.

    A plain ``deadstrip`` is the one disposition never compared against the
    cartridge, and the ``elif homes.get(sym)`` arm below cannot catch a COINED
    class name: ``homes`` is keyed on the symbols.txt spelling while _ZTI/_ZTS
    are LENGTH-PREFIXED mangled strings, so a coined name misses on both the
    prefix and the body and the miss reads as "the ROM has no such record".
    This path must refuse on its own -- ``_isolate`` returns before rombuild's
    copy of the guard ever runs.
    """
    if not _toolchain():
        return
    obj = _compile_tu_fixture(
        "struct B { virtual ~B() {} };\n"
        "struct D : B { virtual ~D() {} virtual void f(); };\n"
        "void D::f() {}\n")
    assert obj is not None
    symbols = {s["name"]: s for s in tubuild.elf_inventory(obj)["symbols"]
               if s["name"] and not s["name"].startswith("$")}
    duplicates = sorted(n for n, s in symbols.items()
                        if s["type"] == "STT_FUNC" and n.startswith("_ZN1BD"))
    data = sorted(n for n, s in symbols.items()
                  if s["type"] == "STT_OBJECT"
                  and n.startswith(("_ZTV", "_ZTI", "_ZTS")))
    assert data, "fixture must emit RTTI records"
    licensed = sorted(n for n, s in symbols.items()
                      if s["type"] == "STT_FUNC" and n not in duplicates)
    policy = [
        {"symbol": n, "disposition": "deadstrip-duplicate",
         "reason": "vague base destructor with a canonical cartridge copy"}
        for n in duplicates
    ] + [
        {"symbol": n, "disposition": "deadstrip", "reason": "no ROM symbol"}
        for n in data
    ]
    homes = {n: [("arm9", 0x02010000 + i * 0x20)]
             for i, n in enumerate(duplicates)}
    entry = {"functions": [{"symbol": n} for n in licensed],
             "data": [], "bss": [], "compiler_only_output": policy}

    out, _report, reasons = tubuild.apply_compiler_only_policy(
        obj, entry, homes=homes)
    assert out is None
    assert all(any(n in r and "never compared against the cartridge" in r
                   for r in reasons) for n in data), reasons


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


import tubuild


def test_linkcheck_compile_passes_all_production_object_policies():
    original_policies = tubuild.RB.compiler_only_policies
    original_intact = tubuild.RB.intact_tu_policies
    original_compile = tubuild.RB.compile_one
    policy = {"src/actors/Promoted.cpp": {"deadstrip": ["helper"]}}
    intact = {"src/actors/Promoted.cpp": {"id": "ov047/Promoted"}}
    seen = []

    try:
        tubuild.RB.compiler_only_policies = lambda enrolled: (
            policy if list(enrolled) == ["src/actors/Promoted.cpp"] else None)
        tubuild.RB.intact_tu_policies = lambda enrolled: (
            intact if list(enrolled) == ["src/actors/Promoted.cpp"] else None)

        def fake_compile(rel, vers, cache, init_srcs, syms, build_root=None,
                         compiler_only=None, intact_tus=None):
            seen.append((rel, build_root, compiler_only, intact_tus))
            return rel, None, "hit"

        tubuild.RB.compile_one = fake_compile
        failures, outcomes = tubuild.compile_linkcheck_sources(
            ["src/actors/Promoted.cpp"], {}, None, set(), {}, pathlib.Path("scratch"), 1)
    finally:
        tubuild.RB.compiler_only_policies = original_policies
        tubuild.RB.intact_tu_policies = original_intact
        tubuild.RB.compile_one = original_compile

    assert failures == []
    assert outcomes["hit"] == 1
    assert seen == [("src/actors/Promoted.cpp", pathlib.Path("scratch"),
                     policy, intact)]


def test_strict_control_demotes_only_requested_complete_sources():
    with tempfile.TemporaryDirectory() as td:
        root = pathlib.Path(td)
        path = root / "overlays/ov047/delinks.txt"
        path.parent.mkdir(parents=True)
        path.write_text(
            "src/actors/Promoted.cpp:\n"
            "    complete\n"
            "    .text start:0x1000 end:0x1010\n"
            "    .data start:0x2000 end:0x2010\n\n"
            "src/Other.cpp:\n"
            "    complete\n"
            "    .text start:0x1010 end:0x1020\n",
            encoding="utf-8")

        demoted, errors = tubuild.demote_complete_sources(
            root, ["src/actors/Promoted.cpp"])

        assert errors == []
        assert demoted == ["src/actors/Promoted.cpp"]
        text = path.read_text(encoding="utf-8")
        promoted = text.split("src/Other.cpp:", 1)[0]
        assert "complete" not in promoted
        assert ".text start:0x1000 end:0x1010" in promoted
        assert ".data start:0x2000 end:0x2010" in promoted
        assert "src/Other.cpp:\n    complete" in text


def test_strict_control_compile_does_not_re_admit_demoted_intact_tus():
    original_intact = tubuild.RB.intact_tu_policies
    original_compile = tubuild.RB.compile_one
    seen = []
    try:
        tubuild.RB.intact_tu_policies = lambda _enrolled: (_ for _ in ()).throw(
            AssertionError("demoted intact policy was recomputed"))

        def fake_compile(rel, vers, cache, init_srcs, syms, build_root=None,
                         compiler_only=None, intact_tus=None):
            seen.append(intact_tus)
            return rel, None, "hit"

        tubuild.RB.compile_one = fake_compile
        failures, outcomes = tubuild.compile_linkcheck_sources(
            ["src/Other.cpp"], {}, None, set(), {}, pathlib.Path("scratch"), 1,
            intact_tus_override={})
    finally:
        tubuild.RB.intact_tu_policies = original_intact
        tubuild.RB.compile_one = original_compile

    assert failures == []
    assert outcomes["hit"] == 1
    assert seen == [{}]


def test_strict_control_refuses_a_source_that_was_not_complete():
    with tempfile.TemporaryDirectory() as td:
        root = pathlib.Path(td)
        path = root / "delinks.txt"
        path.write_text(
            "src/actors/Promoted.cpp:\n"
            "    .text start:0x1000 end:0x1010\n",
            encoding="utf-8")
        demoted, errors = tubuild.demote_complete_sources(
            root, ["src/actors/Promoted.cpp"])
        assert demoted == []
        assert errors == [
            "src/actors/Promoted.cpp: delinks entry is not complete"]


def test_linkcheck_symbol_verdict_uses_the_stock_failure_inventory():
    assert tubuild.linkcheck_symbol_verdict(True, False, None)
    assert tubuild.linkcheck_symbol_verdict(False, False, [])
    assert not tubuild.linkcheck_symbol_verdict(False, False, ["new error"])
    assert tubuild.linkcheck_symbol_verdict(False, True, None)
    assert not tubuild.linkcheck_symbol_verdict(False, False, None)


def test_vtable_storage_address_requires_an_explicit_consistent_bias():
    if not _toolchain():
        return
    obj = _compile_tu_fixture(
        'extern "C" int _ZTV1V[3] = {1, 2, 3};\n'
        'extern "C" int* vptr = _ZTV1V + 2;\n')
    start = 0x4000
    data, error = tubuild.section_contribution(obj, ".data", start)
    assert error is None
    emitted = data["symbols"]["_ZTV1V"]
    pointer = data["symbols"]["vptr"]
    reloc = next(r for r in data["relocs"] if r["symbol"] == "_ZTV1V")
    source = start + reloc["offset"]
    public = emitted["address"] + 8
    claims = [{"name": ".data", "start": start,
               "end": start + len(data["bytes"])}]

    def run(row):
        entry = {"module": "ov999", "functions": [], "data": [row, {
                     "symbol": "vptr", "address": hex(pointer["address"]),
                     "size": hex(pointer["size"])}],
                 "bss": [], "relocations": [{
                     "section": ".data", "source": hex(source),
                     "type": "R_ARM_ABS32", "kind": "load", "symbol": "_ZTV1V",
                     "addend": reloc["addend"], "target_module": "ov999",
                     "target_address": hex(public)}]}
        return tubuild.verify_owned_sections(
            obj, entry, claims, name_index={},
            config_relocs={"ov999": {source: ("load", public, "ov999")}},
            sym_index={}, target_reader=lambda _m, _s, _n: data["bytes"],
            symbol_homes={}, bss_boundaries=set())

    implicit = run({"symbol": "_ZTV1V", "address": hex(public),
                    "size": hex(emitted["size"])})
    assert not implicit["ok"]
    assert any("emitted address" in reason for reason in implicit["errors"])

    explicit = run({"symbol": "_ZTV1V", "address": hex(public),
                    "emitted_storage_address": hex(emitted["address"]),
                    "address_point_bias": "0x8", "size": hex(emitted["size"])})
    assert explicit["ok"], explicit["errors"]

    inconsistent = run({"symbol": "_ZTV1V", "address": hex(public),
                        "emitted_storage_address": hex(emitted["address"]),
                        "address_point_bias": "0x4", "size": hex(emitted["size"])})
    assert not inconsistent["ok"]
    assert any("does not equal public address" in reason
               for reason in inconsistent["errors"])


def test_bss_without_independent_configured_symbols_and_boundaries_is_inferred_only():
    if not _toolchain():
        return
    obj = _compile_tu_fixture('extern "C" int invented_bss = 0;\n')
    start = 0x5000
    bss, error = tubuild.section_contribution(obj, ".bss", start)
    assert error is None
    row = bss["symbols"]["invented_bss"]
    entry = {"module": "ov999", "functions": [], "data": [], "relocations": [],
             "bss": [{"symbol": "invented_bss", "address": hex(row["address"]),
                      "size": hex(row["size"])}]}
    result = tubuild.verify_owned_sections(
        obj, entry, [{"name": ".bss", "start": start,
                     "end": start + len(bss["bytes"])}],
        name_index={}, config_relocs={"ov999": {}}, sym_index={},
        target_reader=lambda *_args: None, symbol_homes={}, bss_boundaries=set())
    assert not result["ok"]
    assert any("no independent ov999 symbols.txt home" in reason
               for reason in result["errors"])
    assert any("boundary anchors" in reason for reason in result["errors"])


def test_object_audit_makes_an_extra_unclaimed_object_and_section_fatal():
    if not _toolchain():
        return
    obj = _compile_tu_fixture('extern "C" int unexpected_data = 7;\n')
    entry = {"module": "ov999", "functions": [], "sections": [
        {"name": ".text", "start": "0x1000", "end": "0x1004"}],
        "data": [], "bss": []}
    rows, extra, _emitted, order_ok = tubuild.audit_tu_object(
        obj, entry, 0x1000, 0x1004, ranges={})
    reasons = tubuild.object_audit_refusals(rows, extra, order_ok)
    assert any("unlicensed defined symbol unexpected_data" in r for r in reasons)
    assert any("unlicensed content section .data" in r for r in reasons)


def test_vague_inherited_rtti_coalescing_remains_explicitly_unsupported():
    """Two TUs emit the same STB_LOPROC base metadata; linker behavior is not a license."""
    if not _toolchain():
        return
    common = "struct Base { virtual void f() {} };\n"
    a = _compile_tu_fixture(common + "struct A : Base { virtual void a(); }; void A::a() {}\n")
    b = _compile_tu_fixture(common + "struct B : Base { virtual void b(); }; void B::b() {}\n")
    inv_a, inv_b = tubuild.elf_inventory(a), tubuild.elf_inventory(b)
    vague_a = {s["name"] for s in inv_a["symbols"] if s["bind"] == "STB_LOPROC"}
    vague_b = {s["name"] for s in inv_b["symbols"] if s["bind"] == "STB_LOPROC"}
    inherited = sorted(n for n in vague_a & vague_b if n.startswith(("_ZTI", "_ZTS", "_ZTV")))
    assert inherited, "fixture must measure duplicate inherited vague RTTI/vtable output"

    # Measure the linker too: it accepts the two objects and coalesces each shared
    # vague definition to one output definition.  That fact is deliberately evidence,
    # not permission for the ownership gate below to discard either input definition.
    with tempfile.TemporaryDirectory() as td:
        td = pathlib.Path(td)
        ao, bo, linked = td / "a.o", td / "b.o", td / "linked.o"
        ao.write_bytes(a)
        bo.write_bytes(b)
        objects = td / "objects.txt"
        objects.write_text(f"{ao}\n{bo}\n", encoding="utf-8")
        runtime_vtables = set()
        for raw in (a, b):
            elf = tubuild.ELFFile(tubuild.io.BytesIO(raw))
            symtab = elf.get_section_by_name(".symtab")
            runtime_vtables.update(s.name for s in symtab.iter_symbols()
                                   if s["st_shndx"] == "SHN_UNDEF"
                                   and s.name.startswith("_ZTV"))
        runtime_defs = "\n".join(
            f"  {name} = 0x02010000;" for name in sorted(runtime_vtables))
        lcf = td / "fixture.lcf"
        lcf.write_text(
            "MEMORY { TEST : ORIGIN = 0x02000000 > linked.bin }\n"
            f"SECTIONS {{\n{runtime_defs}\n.fixture : {{\n"
            "  a.o(.text) b.o(.text) a.o(.data) b.o(.data)\n"
            "} > TEST }\n", encoding="utf-8")
        cmd = [*tubuild.RB.launcher(),
               str(tubuild.RB.MW / tubuild.RB.LD_VERSION / "mwldarm.exe"),
               "-proc", "arm946e", "-nostdlib", "-interworking", "-nodead",
               "-m", "_ZN1A1aEv",
               f"@{objects}", str(lcf), "-o", str(linked)]
        result = subprocess.run(cmd, capture_output=True, text=True, cwd=td)
        assert result.returncode == 0, result.stdout + result.stderr
        linked_inv = tubuild.elf_inventory(linked.read_bytes())
        linked_names = [s["name"] for s in linked_inv["symbols"]]
        assert all(linked_names.count(name) == 1 for name in inherited)

    entry = {"module": "ov999", "functions": [], "sections": [
        {"name": ".text", "start": "0x1000", "end": "0x1004"}],
        "data": [], "bss": []}
    rows, extra, _emitted, order_ok = tubuild.audit_tu_object(
        a, entry, 0x1000, 0x1004, ranges={})
    reasons = tubuild.object_audit_refusals(rows, extra, order_ok)
    assert any(any(name in reason for name in inherited) for reason in reasons), \
        "vague duplicate output must remain fatal until exact coalescing is implemented"


def test_exact_vague_externalization_requires_canonical_bytes_relocs_and_home():
    if not _toolchain():
        return
    import copy

    obj, entry, homes, cfg, targets, reader, name_index = \
        _vague_externalization_fixture()
    out, report, reasons = tubuild.apply_externalized_output_policy(
        obj, entry, homes=homes, config_relocs=cfg, target_reader=reader,
        name_index=name_index)
    assert reasons == [] and out is not None
    assert report["externalized"] == ["_ZTI4Base", "_ZTS4Base"]
    assert all(row["ok"] for row in report["verification"]["rows"])
    inv = tubuild.elf_inventory(out)
    defined = {row["name"] for row in inv["symbols"]}
    assert "_ZTI4Base" not in defined and "_ZTS4Base" not in defined

    bad_homes = dict(homes, _ZTI4Base=[("arm9", 0x5004)])
    out, _report, reasons = tubuild.apply_externalized_output_policy(
        obj, entry, homes=bad_homes, config_relocs=cfg, target_reader=reader,
        name_index=name_index)
    assert out is None and any("unique canonical home" in r for r in reasons)

    damaged = dict(targets)
    zts_key = next(key for key in damaged if key[1] == 0x5100)
    zts = bytearray(damaged[zts_key])
    zts[0] ^= 0xff
    damaged[zts_key] = bytes(zts)
    out, _report, reasons = tubuild.apply_externalized_output_policy(
        obj, entry, homes=homes, config_relocs=cfg,
        target_reader=lambda module, address, size: damaged.get((module, address, size)),
        name_index=name_index)
    assert out is None and any("non-relocated byte" in r for r in reasons)

    wrong_addend = copy.deepcopy(entry)
    wrong_addend["externalized_output"][0]["relocations"][0]["addend"] += 4
    out, _report, reasons = tubuild.apply_externalized_output_policy(
        obj, wrong_addend, homes=homes, config_relocs=cfg, target_reader=reader,
        name_index=name_index)
    assert out is None and any("addend" in r for r in reasons)

    wrong_cfg = copy.deepcopy(cfg)
    source = next(iter(wrong_cfg["arm9"]))
    kind, destination, module = wrong_cfg["arm9"][source]
    wrong_cfg["arm9"][source] = (kind, destination + 4, module)
    out, _report, reasons = tubuild.apply_externalized_output_policy(
        obj, entry, homes=homes, config_relocs=wrong_cfg, target_reader=reader,
        name_index=name_index)
    assert out is None and any("canonical relocation" in r for r in reasons)

    wrong_names = dict(name_index)
    symbol = entry["externalized_output"][0]["relocations"][0]["symbol"]
    module, address = wrong_names[symbol]
    wrong_names[symbol] = (module, address + 4)
    out, _report, reasons = tubuild.apply_externalized_output_policy(
        obj, entry, homes=homes, config_relocs=cfg, target_reader=reader,
        name_index=wrong_names)
    assert out is None and any("resolves to" in r for r in reasons)

    unresolved = dict(name_index)
    del unresolved[symbol]
    out, _report, reasons = tubuild.apply_externalized_output_policy(
        obj, entry, homes=homes, config_relocs=cfg, target_reader=reader,
        name_index=unresolved)
    assert out is None and any("is unresolved" in r for r in reasons)


def test_exact_vague_externalization_rejects_wrong_binding_and_vtables():
    if not _toolchain():
        return
    import io
    import struct
    from elftools.elf.elffile import ELFFile

    obj, entry, homes, cfg, _targets, reader, name_index = \
        _vague_externalization_fixture()
    raw = bytearray(obj)
    elf = ELFFile(io.BytesIO(obj))
    symtab = elf.get_section_by_name(".symtab")
    index = next(i for i, sym in enumerate(symtab.iter_symbols())
                 if sym.name == "_ZTI4Base")
    # Elf32_Sym st_info: bind in the high nibble, type in the low.  Make this a
    # strong STB_GLOBAL/STT_OBJECT definition without disturbing any other field.
    raw[symtab.header["sh_offset"] + index * 16 + 12] = 0x11
    out, _report, reasons = tubuild.apply_externalized_output_policy(
        bytes(raw), entry, homes=homes, config_relocs=cfg, target_reader=reader,
        name_index=name_index)
    assert out is None and any("STB_GLOBAL/STT_OBJECT" in r for r in reasons)

    vtable_entry = {"externalized_output": [{
        "symbol": "_ZTV4Base", "disposition": "canonical-import",
        "section": ".data", "binding": "STB_LOPROC", "size": 8,
        "canonical_module": "arm9", "canonical_address": 0x5000,
        "reason": "must fail closed", "relocations": [],
    }]}
    _rows, reasons = tubuild.manifest_externalized_output(vtable_entry)
    assert any("not an _ZTI/_ZTS" in reason for reason in reasons)


def test_scratch_and_no_rom_results_are_not_promotion_ready():
    assert tubuild.classify_link_result(True, True, None, False) == \
        "module-data-verified"
    assert tubuild.classify_link_result(True, True, True, True) == \
        "scratch-data-verified"
    assert tubuild.classify_link_result(True, True, True, False) == "data-verified"

    ready = {"status": "link-verified", "sections": [{"name": ".text"}],
             "verification": {"linkcheck": {
                 "result": "link-verified", "phases": {"rom": True},
                 "rom": {"matchesStockRom": True}}}}
    assert tubuild.promotion_refusals(ready) == []
    assert any("non-.text" in reason for reason in tubuild.promotion_refusals(
        dict(ready, sections=[{"name": ".text"}, {"name": ".data"}])))
    assert not any("compiler_only_output" in reason for reason in tubuild.promotion_refusals(
        dict(ready, compiler_only_output=[{
            "symbol": "D2", "disposition": "deadstrip", "reason": "unused variant"}])))
    assert any("externalized_output" in reason for reason in tubuild.promotion_refusals(
        dict(ready, externalized_output=[{"symbol": "_ZTI4Base"}])))
    no_rom = dict(ready, verification={"linkcheck": {
        "result": "link-verified", "phases": {"rom": None}, "rom": {}}})
    assert any("full-ROM phase" in reason for reason in tubuild.promotion_refusals(no_rom))
    null_record = dict(ready, verification={"linkcheck": {
        "result": "failed", "phases": {"rom": False}, "rom": None}})
    assert any("not proven identical" in reason
               for reason in tubuild.promotion_refusals(null_record))


def test_partitioned_delinks_addition_is_gap_only_and_same_section_name_only():
    text = (
        "    .text start:0x00001000 end:0x00001100 kind:code align:4\n"
        "    .data start:0x00002000 end:0x00002100 kind:data align:4\n"
        "\n"
        "src/f.cpp:\n    complete\n"
        "    .text start:0x00001000 end:0x00001010\n\n"
        "src/g.cpp:\n    complete\n"
        "    .text start:0x00001010 end:0x00001020\n")
    entry = {"source": "src_tu/T.cpp", "functions": [
        {"legacy_source": "src/f.cpp"}, {"legacy_source": "src/g.cpp"}]}
    claims = [{"name": ".text", "module_section": ".text",
               "start": 0x1000, "end": 0x1020},
              {"name": ".data", "module_section": ".data",
               "start": 0x2040, "end": 0x2050}]
    with tempfile.TemporaryDirectory() as td:
        path = pathlib.Path(td) / "delinks.txt"
        path.write_text(text, encoding="utf-8")
        replaced, reasons = tubuild.add_partitioned_tu_entry(
            path, 0x1000, 0x1020, entry["source"],
            [row["legacy_source"] for row in entry["functions"]], claims)
        assert reasons == [] and replaced == ["src/f.cpp", "src/g.cpp"]
        written = path.read_text(encoding="utf-8")
        assert written.count("src/f.cpp:") == 1 and written.count("src/g.cpp:") == 1
        assert "src_tu/T.cpp:\n    complete\n    .data start:0x00002040" in written
        assert "src_tu/T.cpp:\n    complete\n    .text" not in written

        mapped = pathlib.Path(td) / "mapped.txt"
        mapped.write_text(text, encoding="utf-8")
        bad_claims = [claims[0], {"name": ".rodata", "module_section": ".data",
                                  "start": 0x2040, "end": 0x2050}]
        before = mapped.read_bytes()
        replaced, reasons = tubuild.add_partitioned_tu_entry(
            mapped, 0x1000, 0x1020, entry["source"],
            [row["legacy_source"] for row in entry["functions"]], bad_claims)
        assert replaced is None
        assert any("retargeting" in reason for reason in reasons)
        assert mapped.read_bytes() == before


def test_partitioned_artifact_audit_uses_exact_selectors_and_object_paths():
    entry = {"source": "src_tu/actors/T.cpp", "functions": [
        {"legacy_source": "src/f.cpp"}, {"legacy_source": "src/g.cpp"}]}
    claims = [{"name": ".text", "module_section": ".text",
               "start": 0x1000, "end": 0x1020},
              {"name": ".data", "module_section": ".data",
               "start": 0x2000, "end": 0x2010}]
    with tempfile.TemporaryDirectory() as td:
        root = pathlib.Path(td)
        lcf, objects = root / "arm9.lcf", root / "objects.txt"
        expected_paths = [root / "src" / "f.o", root / "src" / "g.o",
                          root / "src_tu" / "actors" / "T.o"]
        objects.write_text("\n".join(str(path.resolve()) for path in expected_paths) + "\n",
                           encoding="utf-8")
        good_lines = ["f.o(.text)", "g.o(.text)", "T.o(.data)"]
        lcf.write_text("\n".join(good_lines) + "\n", encoding="utf-8")
        result = tubuild.validate_partitioned_link_artifacts(lcf, objects, entry, claims)
        assert result["ok"], result["errors"]
        assert result["observedTuSelectors"] == ["T.o(.data)"]
        assert result["objectCount"] == 3
        assert len(result["selectorListSha256"]) == 64
        assert len(result["objectListSha256"]) == 64

        lcf.write_text("Xf.o(.text)\ng.o(.text)\nT.o(.data)\n", encoding="utf-8")
        result = tubuild.validate_partitioned_link_artifacts(lcf, objects, entry, claims)
        assert not result["ok"]
        assert any("0 exact lines f.o(.text)" in reason for reason in result["errors"])

        lcf.write_text("\n".join(good_lines + ["T.o(.ctor)"]) + "\n", encoding="utf-8")
        result = tubuild.validate_partitioned_link_artifacts(lcf, objects, entry, claims)
        assert not result["ok"]
        assert any("TU selector set" in reason for reason in result["errors"])

        lcf.write_text("\n".join(good_lines) + "\n", encoding="utf-8")
        objects.write_text("\n".join(str(path.resolve()) for path in expected_paths[:-1])
                           + f"\n{(root / 'wrong' / 'T.o').resolve()}\n", encoding="utf-8")
        result = tubuild.validate_partitioned_link_artifacts(lcf, objects, entry, claims)
        assert not result["ok"]
        assert any("exact paths" in reason for reason in result["errors"])

        objects.write_text("\n".join(str(path.resolve()) for path in expected_paths)
                           + f"\n{(root / 'other' / 'T.o').resolve()}\n", encoding="utf-8")
        result = tubuild.validate_partitioned_link_artifacts(lcf, objects, entry, claims)
        assert not result["ok"]
        assert any("2 objects named T.o" in reason for reason in result["errors"])


def test_partitioned_vtable_rebias_needs_one_unique_configured_public_home():
    entry = {"module": "ov999", "functions": [], "data": [{
        "symbol": "_ZTV1P", "address": "0x2008",
        "emitted_storage_address": "0x2000", "address_point_bias": "0x8",
        "size": "0x20"}], "bss": []}
    claims = [{"name": ".data", "start": 0x2000, "end": 0x2020}]
    original = tubuild.all_symbol_homes
    try:
        tubuild.all_symbol_homes = lambda: {"_ZTV1P": [("ov999", 0x2008)]}
        policies, reasons = tubuild.partition_vtable_rebiases(entry, claims)
        assert reasons == []
        assert policies["_ZTV1P"]["publicAddress"] == 0x2008

        tubuild.all_symbol_homes = lambda: {
            "_ZTV1P": [("ov999", 0x2008), ("arm9", 0x2008)]}
        _policies, reasons = tubuild.partition_vtable_rebiases(entry, claims)
        assert any("one unique configured public home" in reason for reason in reasons)

        alias_entry = {"module": "ov999", "functions": [],
                       "compiler_only_output": [{"symbol": "_ZN1PD2Ev",
                                                  "disposition": "deadstrip",
                                                  "reason": "compiler-only D2"}],
                       "data": [{
                           "symbol": "_ZTV1P", "address": "0x2008",
                           "emitted_storage_address": "0x2000",
                           "address_point_bias": "0x8", "size": "0x20",
                           "storage_alias": {
                               "symbol": "data_2000", "address": "0x2000",
                               "size": "0x8", "binding": "STB_GLOBAL",
                               "type": "STT_OBJECT", "visibility": "STV_DEFAULT",
                               "reuse_compiler_only_symbol": "_ZN1PD2Ev"}}], "bss": []}
        baseline = {
            "data_2000": [{"address": 0x2000, "size": 8,
                           "binding": "STB_GLOBAL", "type": "STT_OBJECT",
                           "visibility": "STV_DEFAULT",
                           "sectionIndex": 4, "section": ".data"}],
            "_ZTV1P": [{"address": 0x2008, "size": 0x18,
                        "binding": "STB_GLOBAL", "type": "STT_OBJECT",
                        "visibility": "STV_DEFAULT",
                        "sectionIndex": 4, "section": ".data"}],
        }
        tubuild.all_symbol_homes = lambda: {
            "_ZTV1P": [("ov999", 0x2008)], "data_2000": [("ov999", 0x2000)]}
        policies, reasons = tubuild.partition_vtable_rebiases(
            alias_entry, claims, baseline_symbols=baseline, baseline_sha256="a" * 64)
        assert reasons == []
        alias = policies["_ZTV1P"]["storageAlias"]
        assert alias["donor"] == "_ZN1PD2Ev"
        assert alias["baseline"]["elfSha256"] == "a" * 64
        assert tubuild.manifest_storage_alias_rows(alias_entry)[0][1]["size"] == "0x8"

        wrong = {key: [dict(row) for row in value] for key, value in baseline.items()}
        wrong["data_2000"][0]["size"] = 0
        _policies, reasons = tubuild.partition_vtable_rebiases(
            alias_entry, claims, baseline_symbols=wrong, baseline_sha256="b" * 64)
        assert any("baseline metadata differs" in reason for reason in reasons)

        original_linked = tubuild.linked_symbol_rows
        try:
            tubuild.linked_symbol_rows = lambda _path, _names: (baseline, None)
            proof = tubuild.verify_linked_storage_aliases("ignored.o", policies)
            assert proof["ok"] and proof["rows"][0]["exact"]
            tubuild.linked_symbol_rows = lambda _path, _names: (wrong, None)
            proof = tubuild.verify_linked_storage_aliases("ignored.o", policies)
            assert not proof["ok"]
        finally:
            tubuild.linked_symbol_rows = original_linked
    finally:
        tubuild.all_symbol_homes = original


def test_partition_baseline_evidence_is_content_bound_not_mtime_bound():
    with tempfile.TemporaryDirectory() as td:
        root = pathlib.Path(td)
        config = root / "config"
        tracked = root / "tracked-config"
        rom_inputs = root / "rom-inputs"
        config.mkdir()
        tracked.mkdir()
        rom_inputs.mkdir()
        cfg = config / "symbols.txt"
        tracked_cfg = tracked / "symbols.txt"
        (rom_inputs / "header.yaml").write_bytes(b"ROM")
        linked, dsd, linker = root / "base.o", root / "dsd.exe", root / "mwld.exe"
        control_tool = root / "analysis.py"
        cfg.write_bytes(b"one")
        tracked_cfg.write_bytes(b"tracked-one")
        linked.write_bytes(b"ELF")
        dsd.write_bytes(b"DSD")
        linker.write_bytes(b"MWL")
        control_tool.write_bytes(b"ANALYZE")

        def fingerprint():
            return tubuild.partition_baseline_fingerprints(
                linked, config, dsd_path=dsd, linker_path=linker,
                rom_inputs=rom_inputs, control_tools=[control_tool],
                tracked_config_root=tracked)

        def validate(report):
            return tubuild.validate_partition_baseline_evidence(
                report, linked, config, dsd_path=dsd, linker_path=linker,
                rom_inputs=rom_inputs, control_tools=[control_tool],
                tracked_config_root=tracked)

        evidence = fingerprint()
        report = {"baselineEvidence": evidence}
        digest, error = validate(report)
        assert error is None and digest == evidence["linkedElfSha256"]

        stamp = cfg.stat().st_mtime_ns
        cfg.write_bytes(b"two")
        os.utime(cfg, ns=(stamp, stamp))
        _digest, error = validate(report)
        assert "configArm9Sha256" in error

        cfg.write_bytes(b"one")
        linked_stamp = linked.stat().st_mtime_ns
        linked.write_bytes(b"BAD")
        os.utime(linked, ns=(linked_stamp, linked_stamp))
        _digest, error = validate(report)
        assert "linkedElfSha256" in error

        linked.write_bytes(b"ELF")
        (rom_inputs / "header.yaml").write_bytes(b"CHANGED")
        _digest, error = validate(report)
        assert "romInputsSha256" in error

        (rom_inputs / "header.yaml").write_bytes(b"ROM")
        control_tool.write_bytes(b"CHANGED")
        _digest, error = validate(report)
        assert "controlToolsSha256" in error

        control_tool.write_bytes(b"ANALYZE")
        _digest, error = validate(report)
        assert error is None


def test_partition_baseline_evidence_binds_the_tracked_config_too():
    """The scratch copy and config/arm9 are two different facts; both are recorded.

    `linkcheck --baseline` links out of a MUTATED scratch copy of config/arm9, so the
    scratch tree is what the report has to bind to in order to describe its own inputs.
    Binding to that alone regresses what the tracked hash used to catch: config/arm9
    can move afterwards and the preserved scratch copy -- untouched by definition --
    still matches, so every consumer keeps calling the baseline current. The case below
    is exactly that: the scratch tree is left byte-identical and ONLY the tracked tree
    changes.
    """
    with tempfile.TemporaryDirectory() as td:
        root = pathlib.Path(td)
        scratch, tracked, rom_inputs = (root / "scratch-config", root / "tracked-config",
                                        root / "rom-inputs")
        for d in (scratch, tracked, rom_inputs):
            d.mkdir()
        # Different bytes on purpose: demote_complete_sources rewrites the scratch copy,
        # so in the real baseline these two trees are NOT equal and the two hashes are
        # independent values rather than one value written twice.
        (scratch / "symbols.txt").write_bytes(b"scratch (demoted)")
        (tracked / "symbols.txt").write_bytes(b"tracked")
        (rom_inputs / "header.yaml").write_bytes(b"ROM")
        linked, dsd, linker = root / "base.o", root / "dsd.exe", root / "mwld.exe"
        control_tool = root / "analysis.py"
        for path, blob in ((linked, b"ELF"), (dsd, b"DSD"), (linker, b"MWL"),
                           (control_tool, b"ANALYZE")):
            path.write_bytes(blob)

        def validate(report):
            return tubuild.validate_partition_baseline_evidence(
                report, linked, scratch, dsd_path=dsd, linker_path=linker,
                rom_inputs=rom_inputs, control_tools=[control_tool],
                tracked_config_root=tracked)

        evidence = tubuild.partition_baseline_fingerprints(
            linked, scratch, dsd_path=dsd, linker_path=linker,
            rom_inputs=rom_inputs, control_tools=[control_tool],
            tracked_config_root=tracked)
        assert evidence["configArm9Sha256"] != evidence["trackedConfigArm9Sha256"]
        report = {"baselineEvidence": evidence}
        assert validate(report)[1] is None

        scratch_before = evidence["configArm9Sha256"]
        stamp = (tracked / "symbols.txt").stat().st_mtime_ns
        (tracked / "symbols.txt").write_bytes(b"tracked, but edited")
        os.utime(tracked / "symbols.txt", ns=(stamp, stamp))
        _digest, error = validate(report)
        assert error is not None and "trackedConfigArm9Sha256" in error
        # The scratch binding is untouched by the drift -- which is why it cannot be the
        # only signal, and why this is not a test of the scratch hash under another name.
        assert tubuild.content_tree_sha256(scratch) == scratch_before
        assert "'configArm9Sha256'" not in error

        (tracked / "symbols.txt").write_bytes(b"tracked")
        assert validate(report)[1] is None

        # A report banked before this key existed cannot answer the question, so it is
        # refused rather than given the benefit of the doubt.
        stale = {"baselineEvidence": {k: v for k, v in evidence.items()
                                      if k != "trackedConfigArm9Sha256"}}
        _digest, error = validate(stale)
        assert error is not None and "trackedConfigArm9Sha256" in error


def test_partitioned_result_gate_requires_every_full_rom_proof():
    good = dict(equivalent=True, data_ok=True, storage_aliases_ok=True,
                artifacts_ok=True, module_ok=True,
                modules_check_ok=True, symbols_ok=True, rom_ok=True,
                rom_identical=True, no_stray_outputs=True)
    assert tubuild.partitioned_link_ready(**good)
    for key in good:
        bad = dict(good)
        bad[key] = None if key == "rom_ok" else False
        assert not tubuild.partitioned_link_ready(**bad), key


def test_partitioned_recorder_is_compact_orthogonal_and_preserves_verified_evidence():
    entry = {"id": "ov999/T", "status": "data-verified"}
    data = {"entries": [entry]}
    report = {
        "result": "partitioned-link-verified", "scratch": "build/tu/T/link-partitioned",
        "partial": {"toolchain": "2004/b56", "flags": ["-O4"], "mergedBytes": 100,
                    "contributionEquivalent": "1/1", "substituted": ["src/f.cpp"],
                    "rows": [{"ordinal": 0, "symbol": "f", "identical": True,
                              "relocCount": 0, "differences": []}]},
        "compilerOnlyOutput": {"requested": ["D2"], "deadstripped": ["D2"],
                               "droppedSections": [4]},
        "externalizedOutput": {"requested": ["_ZTI1B"],
                               "externalized": ["_ZTI1B"], "droppedSections": [6],
                               "verification": {"ok": True, "errors": []}},
        "nontextPartition": {"requestedSections": [".data"],
                             "licensedSymbols": ["_ZTV1T"],
                             "deferredOutputs": [{"symbol": "f", "section": ".text",
                                                  "size": 4}],
                             "liveSections": [{"name": ".data", "size": 16}],
                             "objisolate": {"keeps": [2], "drop": [3],
                                            "externalise": ["f"], "dead": [],
                                            "error": None}},
        "ownedSectionsBeforePartition": {"ok": True, "rows": [], "errors": []},
        "ownedSections": {"ok": True, "rows": [], "errors": []},
        "vtableRebias": {"rebased": [{"symbol": "_ZTV1T", "bias": 8}],
                            "error": None},
        "partitionedObjects": {"rawTuSha256": "a" * 64,
                               "linkedDataSha256": "b" * 64},
        "partitionedArtifacts": {"ok": True, "errors": [],
                                 "expectedTuSelectors": ["T.o(.data)"],
                                 "observedTuSelectors": ["T.o(.data)"],
                                 "expectedLegacyCount": 1, "selectorCount": 100,
                                 "objectCount": 10800, "selectorListSha256": "c" * 64,
                                 "objectListSha256": "d" * 64,
                                 "observedSelectors": ["huge"] * 100,
                                 "observedObjects": ["huge"] * 10800},
        "phases": {"link": {"ok": True}, "rom": {"ok": True}},
        "analysis": {"passed": True}, "symbolsNew": [],
        "rom": {"matchesStockRom": True}, "tuRanges": [],
    }
    original = tubuild.save_manifest
    try:
        tubuild.save_manifest = lambda _data: None
        tubuild._record_partitioned(data, entry, report)
        block = entry["partitioned_link"]
        assert entry["status"] == "data-verified"
        assert block["state"] == "partitioned-link-verified"
        assert block["lastVerified"]["artifactAudit"]["objectCount"] == 10800
        assert "observedObjects" not in block["lastVerified"]["artifactAudit"]
        assert block["lastVerified"]["nontextPartition"]["deferredOutputs"][0]["symbol"] == "f"

        failed = dict(report, result="failed", rom={})
        tubuild._record_partitioned(data, entry, failed)
        assert block["state"] == "partitioned-link-verified"
        assert block["lastAttempt"]["result"] == "failed"
        assert block["lastVerified"]["result"] == "partitioned-link-verified"
        assert entry["status"] == "data-verified"
    finally:
        tubuild.save_manifest = original


def test_partitioned_cli_modes_are_mutually_exclusive_before_any_build():
    code, out = _run("linkcheck", "ov045/PoleLift", "--partial", "--partitioned")
    assert code != 0
    assert "not allowed with argument" in out or "mutually exclusive" in out


def test_record_linkcheck_preserves_all_owned_ranges():
    entry = {"status": "text-verified"}
    report = {
        "result": "scratch-data-verified",
        "scratch": "scratch/path",
        "phases": {"link": {"ok": True}, "rom": {"ok": True}},
        "tuRange": {"section": ".text", "differingBytes": 0},
        "tuRanges": [
            {"section": ".text", "differingBytes": 0},
            {"section": ".data", "differingBytes": 0},
        ],
        "objectAudit": {},
        "symbolsNew": [],
        "analysis": {"moduleFidelity": {"moduleSetSha256": "b" * 64}},
        "rom": {"matchesStockRom": True, "sha256": "a" * 64},
    }
    original = tubuild.save_manifest
    try:
        tubuild.save_manifest = lambda _data: None
        tubuild._record_linkcheck({"entries": [entry]}, entry, report, False)
    finally:
        tubuild.save_manifest = original

    recorded = entry["verification"]["linkcheck"]
    assert recorded["tuRange"] == report["tuRange"]
    assert recorded["tuRanges"] == report["tuRanges"]
    assert recorded["moduleSetSha256"] == "b" * 64

# ---------------------------------------------------------------- create repairs
# The three assemble_shadow_source behaviors proven by six modules of
# hand-assembly (222 byte-verified functions) before being folded into the
# generator: extern "C" BLOCKS around .c-derived members (defect 2), #pragma
# long_calls carried in position and bracketed (positional on 2004/b56 -- the
# ov014 seed), and raw-concatenation fallback instead of refusing a candidate
# (defect 4). Synthetic: no compiler, no ROM -- these test the emitted TEXT.

def _member(cpp, text, pragmas=(), **over):
    d = {"error": None, "cpp": cpp, "includes": [], "pragmas": list(pragmas),
         "macros": [], "externs": [], "shadow_decls": [], "notes": [],
         "function_text": text, "legacy_path": "src/synthetic.c"}
    d.update(over)
    return d


def test_c_member_gets_a_linkage_block_not_a_prefix():
    ord_rows = [(0, "f_lo", 0x1000, 8), (1, "f_hi", 0x1008, 8)]
    parsed = {"f_lo": _member(False, "int f_lo(void) { return 0; }\n"),
              "f_hi": _member(True, "int f_hi() { return 1; }\n")}
    body, _w = tubuild.assemble_shadow_source("t/T", ord_rows, parsed)
    assert 'extern "C" {' in body, "the .c member must be wrapped in a block"
    assert 'extern "C" int f_lo' not in body, \
        "prefixing the first line is defect 2: the linkage lands on a preamble decl"
    assert body.index("f_hi") < body.index("int f_lo"), "source order must be reverse ROM"


def test_long_calls_is_carried_in_position_and_bracketed():
    ord_rows = [(0, "veneer", 0x1000, 8)]
    parsed = {"veneer": _member(False, "int veneer(void) { return g(); }\n",
                                pragmas=["#pragma long_calls on"])}
    body, _w = tubuild.assemble_shadow_source("t/T", ord_rows, parsed)
    on = body.index("#pragma long_calls on")
    off = body.index("#pragma long_calls off")
    assert on < body.index("int veneer") < off, \
        "long_calls must open before its own member and close after it"


def test_file_global_pragmas_are_still_left_out():
    ord_rows = [(0, "f", 0x1000, 8)]
    parsed = {"f": _member(False, "int f(void) { return 0; }\n",
                           pragmas=["#pragma optimize_for_size on"])}
    body, _w = tubuild.assemble_shadow_source("t/T", ord_rows, parsed)
    assert body.count("#pragma optimize_for_size") == 0 or \
        "NOT carried" in body.split("optimize_for_size", 1)[1][:200], \
        "file-global pragmas poison a merged TU and stay advisory-only"


def test_sourceless_member_becomes_a_banner_not_a_refusal():
    ord_rows = [(0, "ghost", 0x1000, 8), (1, "real", 0x1008, 8)]
    parsed = {"ghost": _member(False, "", missing=True),
              "real": _member(True, "int real() { return 1; }\n")}
    body, _w = tubuild.assemble_shadow_source("t/T", ord_rows, parsed)
    assert "SOURCELESS member ghost" in body
    assert "int real()" in body


def test_anonymous_typedefs_key_by_their_trailing_name():
    """`typedef struct { ... } X;` used to key as ('typedef','struct'), so two
    DIFFERENT unnamed types collided and the merger silently dropped one
    (State300 in ov020). The typedef'd trailing identifier is the key now."""
    a = tubuild.split_legacy_source(
        "typedef struct {\n    int x, y, z;\n} Vector3;\nint f(void) { return 0; }\n")
    b = tubuild.split_legacy_source(
        "typedef struct {\n    unsigned char _pad[0x9e];\n    unsigned short counter;\n} State300;\nint g(void) { return 0; }\n")
    (ka, na, _ta), = a["shadow_decls"]
    (kb, nb, _tb), = b["shadow_decls"]
    assert (ka, na) == ("typedef", "Vector3")
    assert (kb, nb) == ("typedef", "State300")
    assert (ka, na) != (kb, nb), "distinct unnamed types must not share a merge key"


def test_forward_decl_folds_into_the_definition_either_order():
    """A bare `struct C;` forward declaration must never oust or conflict with a
    full `struct C { ... };` definition of the same name (RacingPenguin, ov019).
    The definition wins whichever order they arrive in; a forward line that
    carries MORE than the declaration (e.g. a piggybacked typedef) still flags."""
    fwd = ("struct", "C", "struct C;")
    full = ("struct", "C", "struct C { char pad[0x370]; int idx; };")
    rider = ("struct", "C", "struct C; typedef void (C::*PMF)();")

    def run(first, second):
        w = []
        parsed = {"a": {"shadow_decls": [first]}, "b": {"shadow_decls": [second]}}
        rows = [(0, "a", 0, 4), (1, "b", 4, 4)]
        live, dead = tubuild._merge_field(rows, parsed, lambda p: p["shadow_decls"],
                                          lambda i: (i[0], i[1]), "local declaration", w)
        return live, dead, w

    live, dead, w = run(fwd, full)
    assert live == [(("struct", "C"), full)] and not dead and not w, \
        "definition must replace the earlier forward decl silently"
    live, dead, w = run(full, fwd)
    assert live == [(("struct", "C"), full)] and not dead and not w, \
        "a later forward decl must fold into the kept definition silently"
    live, dead, w = run(rider, full)
    assert dead and w, "a forward decl with piggybacked text must still flag"
