"""Tests for tools/tu_manifest_resync.py.

DEPENDENCY-FREE AND TOOLCHAIN-FREE, deliberately. Exactly one function in the
module under test needs mwccarm -- `compile_and_read`, which compiles the TU and
reads the symbols back out of the object. Everything else is pairing, planning
and substitution over dicts and strings, and that is the part that can be wrong
in a way no byte gate notices: a manifest is CONFIGURATION, so a mispaired
symbol does not fail to compile, it licenses the wrong address. The end-to-end
cases below stub `compile_and_read` with a literal symbol set, which is the only
mock in the file, and get the whole of `main()` -- planning, the two-file write,
`--check`'s exit code and the prose report -- with no compiler and no ROM.

WHAT THE TOOL IS FOR, because the tests only make sense against it. mwccarm
names file-local and vague-linkage entities with a POSITIONAL uniquifier --
`@344`, `Bundle$725daMky_c_cpp`. The number is assigned in parse order, so any
edit that adds, removes or reorders an entity renumbers it, INCLUDING an edit to
a shared base header the manifest's own TU never mentions. The shifts compose
additively, so two independently-correct re-syncs of one manifest are mutually
exclusive and the conflict must be regenerated, never resolved. This tool
regenerates it.

THE FIVE THINGS THAT COST A BUILD CYCLE EACH, and the test that pins each:

  1. A re-sync is TWO files. Fixing only `config/tu_manifest.d/**` turns
     `binding rewrite symbol @344 has 0 entries` into `licensed .bss symbol @356
     has no independent ov023 symbols.txt home`. The overlay's symbols.txt must
     carry the same rename in the same commit.
     -> test_writes_both_the_manifest_and_the_overlay_symbol_table

  2. The substitution must be SIMULTANEOUS. ov100 really maps @438->@444 and
     @444->@450 in one manifest; replacing in sequence lands @438 at @450.
     -> test_overlapping_rename_map_is_applied_in_one_pass

  3. The two pairings are different rules. A mangled name carries a SHAPE and
     pairs by it; a bare `@NNN` carries nothing and pairs by ORDERED POSITION,
     which is only safe when section and size agree entry-for-entry.
     -> TestPlanMangled, TestPlanBare

  4. Prose is not policy. A `reason` narrates its own renumbering history, so a
     blanket substitution can turn a true sentence into a false one -- measured,
     not hypothetical: a hand `.replace("$725", "$739")` produced "$739 with
     SharedFilePtr.h included" for a step that really was $725. The tool reports
     every prose site it touched or left behind, and fixes none of them.
     -> TestStaleProse

  5. `--check` must be able to return BOTH answers. A checker that only ever
     agrees with you is where harness bugs survive.
     -> test_check_exits_1_and_writes_nothing / test_check_exits_0_when_in_sync
"""

import contextlib
import io
import json
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

import tu_manifest_resync as R  # noqa: E402


# --- fixture builders -----------------------------------------------------------------

def bundle(n, stem="daMky_c_cpp", variant="D1"):
    """A mangled Bundle destructor, with the Itanium length prefix kept honest.

    The prefix counts the characters after it, so a uniquifier crossing a digit
    boundary ($99 -> $105) moves the prefix too. Spelling it by hand in the
    tests would quietly hide that, which is the case shape() exists for.
    """
    inner = f"Bundle${n}{stem}"
    return f"_ZN{len(inner)}{inner}{variant}Ev"


def manifest(compiler_only=(), bss=(), data=(), **extra):
    entry = {
        "id": "ov023/daObjFm_Battan_c",
        "module": "ov023",
        "source": "src/game/actors/d_a_obj_fm_battan.cpp",
        "compiler_only_output": [
            dict(symbol=s, disposition="deadstrip", reason=r)
            for s, r in compiler_only
        ],
        "bss": [dict(symbol=s, address="0x02112090", size=z, evidence=e)
                for s, z, e in bss],
        "data": [dict(symbol=s, address="0x02111fa0", size=z, evidence=e)
                 for s, z, e in data],
    }
    entry.update(extra)
    return entry


def emitted_bare(*rows):
    """(number, section, size) triples as compile_and_read returns them: sorted."""
    return sorted(rows)


@contextlib.contextmanager
def fake_repo(entry, symbols_txt=None):
    """A tmpdir shaped like the repo, with R.REPO and R.MANIFEST_DIR pointed at it."""
    with tempfile.TemporaryDirectory(prefix="resync-test-") as td:
        root = pathlib.Path(td)
        ident = entry["id"]
        mpath = root / "config" / "tu_manifest.d" / f"{ident}.json"
        mpath.parent.mkdir(parents=True)
        mpath.write_text(json.dumps(entry, indent=2), encoding="utf-8", newline="")
        spath = root / "config" / "arm9" / "overlays" / entry["module"] / "symbols.txt"
        spath.parent.mkdir(parents=True)
        spath.write_text(symbols_txt or "", encoding="utf-8", newline="")
        old_repo, old_dir = R.REPO, R.MANIFEST_DIR
        R.REPO, R.MANIFEST_DIR = root, root / "config" / "tu_manifest.d"
        try:
            yield mpath, spath
        finally:
            R.REPO, R.MANIFEST_DIR = old_repo, old_dir


@contextlib.contextmanager
def stub_compile(mangled, bare=()):
    """Stand in for the one function that needs mwccarm."""
    real = R.compile_and_read
    R.compile_and_read = lambda entry: (set(mangled), sorted(bare))
    try:
        yield
    finally:
        R.compile_and_read = real


def run(argv):
    """main(argv) -> (exit code, stdout)."""
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        code = R.main(argv)
    return code, buf.getvalue()


# --- shape ----------------------------------------------------------------------------

class TestShape(unittest.TestCase):
    """The whole willingness to rewrite rests on this: same shape = same entity."""

    def test_two_numberings_of_one_entity_share_a_shape(self):
        self.assertEqual(R.shape(bundle(725)), R.shape(bundle(739)))

    def test_shape_survives_the_length_prefix_moving(self):
        # $99 -> $105 lengthens the name, so _ZN20 becomes _ZN21. Comparing the
        # raw strings, or blanking only the uniquifier, calls these different
        # entities and the row gets reported MANUAL for no reason.
        two, three = bundle(99), bundle(105)
        self.assertTrue(two.startswith("_ZN20") and three.startswith("_ZN21"), (two, three))
        self.assertEqual(R.shape(two), R.shape(three))

    def test_different_entities_do_not_share_a_shape(self):
        self.assertNotEqual(R.shape(bundle(725)), R.shape(bundle(725, stem="daBmb_c_cpp")))
        self.assertNotEqual(R.shape("@class$432daDsnBase_c_cpp"),
                            R.shape("Bundle$432daDsnBase_c_cpp"))

    def test_destructor_variants_do_not_share_a_shape(self):
        # D1 and D2 differ by one character and mean different functions; this
        # tree has already shipped one D1/D2 name swap.
        self.assertNotEqual(R.shape(bundle(725, variant="D1")),
                            R.shape(bundle(725, variant="D2")))


# --- the mangled pairing ---------------------------------------------------------------

class TestPlanMangled(unittest.TestCase):

    def test_pairs_a_renumbered_symbol_by_shape(self):
        entry = manifest(compiler_only=[(bundle(725), "file-local aggregate dtor")])
        renames, manual = R.plan_mangled(entry, {bundle(739), "_ZN7daMky_cD2Ev"})
        self.assertEqual(renames, {bundle(725): bundle(739)})
        self.assertEqual(manual, [])

    def test_a_row_the_object_still_emits_is_left_alone(self):
        entry = manifest(compiler_only=[(bundle(725), "unchanged")])
        renames, manual = R.plan_mangled(entry, {bundle(725), bundle(739)})
        self.assertEqual((renames, manual), ({}, []))

    def test_no_candidate_is_reported_not_guessed(self):
        entry = manifest(compiler_only=[(bundle(725), "gone")])
        renames, manual = R.plan_mangled(entry, {"_ZN7daMky_cD2Ev"})
        self.assertEqual(renames, {})
        self.assertEqual([m[0] for m in manual], [bundle(725)])
        self.assertIn("shares its shape", manual[0][1])

    def test_two_candidates_are_reported_not_guessed(self):
        # Two same-shape entities in one TU -- two file-local structs with the
        # same name in different function bodies. Position cannot disambiguate
        # them from a set, so the tool must refuse rather than pick.
        entry = manifest(compiler_only=[(bundle(725), "one of two")])
        renames, manual = R.plan_mangled(entry, {bundle(739), bundle(746)})
        self.assertEqual(renames, {})
        self.assertIn("ambiguous, 2 candidates", manual[0][1])

    def test_a_symbol_another_row_already_names_is_not_a_candidate(self):
        # $739 is in the object AND is some other row's live symbol, so the
        # stale $725 must pair with $746, not steal $739.
        entry = manifest(compiler_only=[(bundle(725), "stale"), (bundle(739), "live")])
        renames, manual = R.plan_mangled(entry, {bundle(739), bundle(746)})
        self.assertEqual(renames, {bundle(725): bundle(746)})
        self.assertEqual(manual, [])

    def test_a_name_with_no_uniquifier_is_never_rewritten(self):
        entry = manifest(compiler_only=[("_ZTV7daMky_c", "vague-linkage vtable")])
        renames, manual = R.plan_mangled(entry, {"_ZTI7daMky_c"})
        self.assertEqual(renames, {})
        self.assertIn("no positional uniquifier", manual[0][1])


# --- the bare-@ pairing -----------------------------------------------------------------

class TestManifestBareRows(unittest.TestCase):

    def test_reads_sizes_as_hex_and_tags_the_section(self):
        entry = manifest(bss=[("@343", "0xc", "dtor node"), ("@344", "0x8", "dtor node")],
                         data=[("@222", "0x10", "string literal")])
        self.assertEqual(R.manifest_bare_rows(entry),
                         [(222, ".data", 16), (343, ".bss", 12), (344, ".bss", 8)])

    def test_ignores_rows_whose_symbol_is_not_a_bare_uniquifier(self):
        entry = manifest(bss=[("data_ov023_02112080", "0x8", "resource handle"),
                              ("@343", "0xc", "dtor node")])
        self.assertEqual(R.manifest_bare_rows(entry), [(343, ".bss", 12)])


class TestPlanBare(unittest.TestCase):

    def test_pairs_by_position_when_section_and_size_agree(self):
        entry = manifest(bss=[("@343", "0xc", "collision node"),
                              ("@344", "0xc", "model node")])
        renames, manual = R.plan_bare(entry, emitted_bare((355, ".bss", 12),
                                                          (356, ".bss", 12)))
        self.assertEqual(renames, {"@343": "@355", "@344": "@356"})
        self.assertEqual(manual, [])

    def test_nothing_stale_is_no_rename(self):
        entry = manifest(bss=[("@343", "0xc", "node")])
        self.assertEqual(R.plan_bare(entry, emitted_bare((343, ".bss", 12))), ({}, []))

    def test_a_count_mismatch_refuses_rather_than_zipping_short(self):
        # The object gained a compiler-only object the manifest has no row for.
        # Zipping would silently re-point @344 at the new one.
        entry = manifest(bss=[("@343", "0xc", "a"), ("@344", "0xc", "b")])
        renames, manual = R.plan_bare(entry, emitted_bare((355, ".bss", 12),
                                                          (356, ".bss", 12),
                                                          (357, ".bss", 12)))
        self.assertEqual(renames, {})
        self.assertEqual(len(manual), 2)
        self.assertIn("the object emits 3", manual[0][1])

    def test_a_size_disagreement_refuses(self):
        entry = manifest(bss=[("@343", "0xc", "12-byte node")])
        renames, manual = R.plan_bare(entry, emitted_bare((355, ".bss", 8)))
        self.assertEqual(renames, {})
        self.assertIn("positional pairing rejected", manual[0][1])

    def test_a_section_disagreement_refuses(self):
        entry = manifest(bss=[("@343", "0xc", "bss node")])
        renames, manual = R.plan_bare(entry, emitted_bare((355, ".data", 12)))
        self.assertEqual(renames, {})
        self.assertIn("positional pairing rejected", manual[0][1])


# --- substitution -----------------------------------------------------------------------

class TestSubstitute(unittest.TestCase):

    def test_overlapping_rename_map_is_applied_in_one_pass(self):
        # ov100/daStarGate_c, measured: a uniform +6 across @438..@454, so every
        # old number but the last is also some other row's new number. Sequential
        # replacement walks @438 all the way to @450.
        renames = {"@438": "@444", "@444": "@450", "@450": "@456"}
        out, hits = R.substitute("@438 @444 @450", renames)
        self.assertEqual(out, "@444 @450 @456")
        self.assertEqual(hits, {"@438": 1, "@444": 1, "@450": 1})

    def test_a_longer_number_is_not_a_prefix_match(self):
        out, hits = R.substitute("@4380 @438bar @438", {"@438": "@444"})
        self.assertEqual(out, "@4380 @438bar @444")
        self.assertEqual(hits, {"@438": 1})

    def test_mangled_and_bare_renames_travel_together(self):
        out, _ = R.substitute(f'"{bundle(725)}" "@343"',
                              {bundle(725): bundle(739), "@343": "@355"})
        self.assertEqual(out, f'"{bundle(739)}" "@355"')

    def test_counts_every_occurrence(self):
        _, hits = R.substitute("@343 @343 @343", {"@343": "@355"})
        self.assertEqual(hits, {"@343": 3})

    def test_a_rename_nothing_matches_reports_no_hits(self):
        out, hits = R.substitute("@999", {"@343": "@355"})
        self.assertEqual((out, hits), ("@999", {}))


# --- prose --------------------------------------------------------------------------------

class TestStaleProse(unittest.TestCase):
    """Prose is reported, never fixed. Both arms, because both have happened."""

    @staticmethod
    def call(reason, renames, **arrays):
        entry = manifest(compiler_only=[(bundle(725), reason)], **arrays)
        before = json.dumps(entry, indent=2)
        after, _ = R.substitute(before, renames)
        return R.stale_prose(before, after, renames)

    def test_a_sentence_the_substitution_left_behind_is_STALE(self):
        # The prose names the bare number; the rename replaces whole symbols, so
        # the sentence survives the pass still describing the old numbering.
        rewritten, stale = self.call("this row read $701 until stage 3b",
                                     {bundle(701): bundle(724)})
        self.assertEqual((rewritten, stale), ([], [bundle(701)]))

    def test_a_sentence_the_substitution_edited_is_REWRITTEN(self):
        # The prose spells the FULL symbol, so the blanket replace rewrites the
        # history into "read $724 ... came back as $724" -- a plausible lie.
        # \b sees no boundary between the 1 and the d of daMky, so a word-boundary
        # match misses exactly the spelling that gets edited.
        rewritten, stale = self.call(f"this row read {bundle(701)} until stage 3b",
                                     {bundle(701): bundle(724)})
        self.assertEqual((rewritten, stale), ([bundle(701)], []))

    def test_a_number_that_is_also_a_new_value_is_not_evidence(self):
        # ov100 again: @444 is @438's new home AND @450's old one. Reporting it
        # produced six false NOTEs on one manifest.
        rewritten, stale = self.call("neighbour of @444",
                                     {"@438": "@444", "@444": "@450"})
        self.assertEqual((rewritten, stale), ([], []))

    def test_prose_that_names_no_number_is_silent(self):
        self.assertEqual(self.call("a file-local aggregate destructor",
                                   {bundle(725): bundle(739)}), ([], []))

    def test_evidence_fields_are_prose_too(self):
        # bss/data/symbol_binding_rewrites rows spell their prose `evidence`, not
        # `reason` -- and those are the rows the bare-@ renames touch. Scanning
        # only `reason` sees none of them.
        rewritten, stale = self.call(
            "unrelated", {"@343": "@355"},
            bss=[("@343", "0xc", "the node @343 registers, paired with @999")])
        self.assertEqual((rewritten, stale), (["@343"], []))


# --- the build log ---------------------------------------------------------------------------

class TestIdsFromBuild(unittest.TestCase):
    """All three failure spellings, because fixing one produces the next."""

    LOG = """\
[4/6] mwldarm
  compiler-only symbol(s) not defined: ['@343', '@344']
  binding rewrite symbol @999 has 0 entries
  licensed .bss symbol _ZN21Bundle$777daMky_c_cppD1Ev at 0x02112090 has no home
"""

    def scan(self, log_text, entries):
        with tempfile.TemporaryDirectory(prefix="resync-log-") as td:
            root = pathlib.Path(td)
            for ident, body in entries.items():
                p = root / "manifests" / f"{ident}.json"
                p.parent.mkdir(parents=True, exist_ok=True)
                p.write_text(body, encoding="utf-8", newline="")
            log = root / "build.log"
            log.write_text(log_text, encoding="utf-8", newline="")
            old = R.MANIFEST_DIR
            R.MANIFEST_DIR = root / "manifests"
            try:
                return R.ids_from_build(log)
            finally:
                R.MANIFEST_DIR = old

    def test_finds_the_manifest_behind_each_failure_shape(self):
        got = self.scan(self.LOG, {
            "ov023/daObjFm_Battan_c": '{"bss": [{"symbol": "@343"}]}',
            "ov044/daObjKb1Billboard_c": '{"bss": [{"symbol": "@999"}]}',
            "ov030/daMky_c": '{"compiler_only_output": '
                             '[{"symbol": "_ZN21Bundle$777daMky_c_cppD1Ev"}]}',
            "ov091/daDsnBase_c": '{"bss": [{"symbol": "@111"}]}',
        })
        self.assertEqual(got, ["ov023/daObjFm_Battan_c", "ov030/daMky_c",
                               "ov044/daObjKb1Billboard_c"])

    def test_a_clean_log_names_nothing(self):
        self.assertEqual(self.scan("[4/6] mwldarm\n      ok\n",
                                   {"ov030/daMky_c": '{"bss": []}'}), [])


# --- end to end, with the compiler stubbed --------------------------------------------------

class TestEndToEnd(unittest.TestCase):

    ENTRY = manifest(
        compiler_only=[(bundle(725), "file-local aggregate destructor")],
        bss=[("@343", "0xc", "collision resource destructor-registration node"),
             ("@344", "0xc", "model resource destructor-registration node")],
    )
    SYMS = ("@343 kind:data addr:0x02112090 size:0xc\n"
            "@344 kind:data addr:0x0211209c size:0xc\n"
            "data_ov023_02112080 kind:data addr:0x02112080 size:0x8\n")
    EMITTED = ({bundle(739)}, [(355, ".bss", 12), (356, ".bss", 12)])
    IN_SYNC = ({bundle(725)}, [(343, ".bss", 12), (344, ".bss", 12)])

    def test_writes_both_the_manifest_and_the_overlay_symbol_table(self):
        # THE two-file rule. Writing only the manifest turns `binding rewrite
        # symbol @344 has 0 entries` into `licensed .bss symbol @356 ... has no
        # independent ov023 symbols.txt home`, one build cycle later.
        with fake_repo(self.ENTRY, self.SYMS) as (mpath, spath), stub_compile(*self.EMITTED):
            code, out = run(["ov023/daObjFm_Battan_c"])
            manifest_text = mpath.read_text(encoding="utf-8")
            symbols_text = spath.read_text(encoding="utf-8")
        self.assertEqual(code, 0, out)
        for old in (bundle(725), '"@343"', '"@344"'):
            self.assertNotIn(old, manifest_text, out)
        for new in (bundle(739), '"@355"', '"@356"'):
            self.assertIn(new, manifest_text, out)
        self.assertIn("@355 kind:data addr:0x02112090", symbols_text)
        self.assertIn("@356 kind:data addr:0x0211209c", symbols_text)
        self.assertNotIn("@343", symbols_text)
        self.assertIn("config/arm9/overlays/ov023/symbols.txt", out)

    def test_rewrites_numbers_and_never_adds_or_drops_a_row(self):
        # A genuinely new compiler-only symbol needs a human-written `reason`, so
        # the tool is only ever allowed to renumber what is already there.
        with fake_repo(self.ENTRY, self.SYMS) as (mpath, _), stub_compile(*self.EMITTED):
            run(["ov023/daObjFm_Battan_c"])
            after = json.loads(mpath.read_text(encoding="utf-8"))
        for key in ("compiler_only_output", "bss"):
            self.assertEqual([r.get("reason") or r.get("evidence") for r in after[key]],
                             [r.get("reason") or r.get("evidence")
                              for r in self.ENTRY[key]])

    def test_check_exits_1_and_writes_nothing(self):
        with fake_repo(self.ENTRY, self.SYMS) as (mpath, spath), stub_compile(*self.EMITTED):
            before = (mpath.read_bytes(), spath.read_bytes())
            code, out = run(["--check", "ov023/daObjFm_Battan_c"])
            self.assertEqual((mpath.read_bytes(), spath.read_bytes()), before)
        self.assertEqual(code, 1)
        self.assertIn(f"STALE   {bundle(725)} -> {bundle(739)}", out)
        self.assertIn("STALE   @343 -> @355", out)
        self.assertIn("1 manifest(s) out of sync", out)

    def test_check_exits_0_when_in_sync(self):
        # The other answer. A --check that cannot return 0 proves nothing when
        # it returns 1, and vice versa.
        with fake_repo(self.ENTRY, self.SYMS), stub_compile(*self.IN_SYNC):
            code, out = run(["--check", "ov023/daObjFm_Battan_c"])
        self.assertEqual(code, 0, out)
        self.assertIn("in sync", out)
        self.assertNotIn("STALE", out)

    def test_a_rewritten_tree_then_checks_clean(self):
        # The round trip: rewrite against an emitted set, then --check against
        # that same set must agree the tree is now current.
        with fake_repo(self.ENTRY, self.SYMS), stub_compile(*self.EMITTED):
            self.assertEqual(run(["ov023/daObjFm_Battan_c"])[0], 0)
            code, out = run(["--check", "ov023/daObjFm_Battan_c"])
        self.assertEqual(code, 0, out)

    def test_an_unpairable_row_is_reported_and_the_file_is_untouched(self):
        with fake_repo(self.ENTRY, self.SYMS) as (mpath, _), stub_compile(set(), []):
            before = mpath.read_bytes()
            code, out = run(["ov023/daObjFm_Battan_c"])
            self.assertEqual(mpath.read_bytes(), before)
        self.assertEqual(code, 0)
        self.assertIn(f"MANUAL  {bundle(725)}", out)
        self.assertIn("0 manifest(s) rewritten", out)

    def test_no_ids_is_an_error_not_a_silent_success(self):
        with self.assertRaises(SystemExit) as cm:
            with contextlib.redirect_stderr(io.StringIO()):
                R.main([])
        self.assertNotEqual(cm.exception.code, 0)


if __name__ == "__main__":
    unittest.main()
