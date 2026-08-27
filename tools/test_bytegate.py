"""The byte-gate conjunct of `matched` (policy D).

Two things are worth testing here and they fail differently.

The MANIFEST half is a recorded verdict, so the risk is that the recording goes stale
and nobody notices: a file gets edited, its exclusion silently lapses or silently
persists, and the published count moves for a reason no one can see. So the hash pin,
the lapse, and the staleness report all get direct coverage, and there is a test that
runs the real config/bytegate-known-failures.txt against the real tree -- that one is
the tripwire that fires in CI when somebody touches one of the 18 files.

The DERIVED half is arithmetic over committed config, so the risk is the opposite: an
address-keyed set is easy to apply to the wrong side of the collision. That bug was
live in the first draft of this change and would have refused to count func_01ff8708
(1,776 bytes) the day someone matched it, which is the defect policy D exists to clear.
It has its own test.

Fixtures rather than the real repo for the behavioural tests, because the point is what
happens when a file is FIXED, and that cannot be staged against committed sources.
"""
import hashlib
import json
import pathlib
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import bytegate as BG        # noqa: E402
import chaos_db_ci as CDB    # noqa: E402
import relocs as RL          # noqa: E402
import srcpath as SP         # noqa: E402
import layout_check as LYC   # noqa: E402

BROKEN = "int f(void) { return nosuchfield; }\n"
FIXED = "int f(void) { return 0; }\n"


def put(path: pathlib.Path, text: str) -> str:
    """Write a source file and return the hash of what actually landed on disk.

    write_bytes, and the hash taken from the file rather than the string, because
    Path.write_text opens in text mode and Windows turns every \\n into \\r\\n on the way
    out. Hashing the string instead would disagree with source_hash by two bytes a line,
    on one platform only -- which is the same class of encoding-layer bug that has
    fabricated data corruption in this repo before."""
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(text.encode("utf-8"))
    return BG.source_hash(path)


class Manifest(unittest.TestCase):
    """Parsing, the hash pin, and the lapse."""

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = pathlib.Path(self.tmp.name)
        (self.root / "src").mkdir()
        (self.root / "config").mkdir()
        self.man = self.root / "config" / "bytegate-known-failures.txt"
        self._saved = BG.REPO
        BG.REPO = self.root
        self.addCleanup(self.tmp.cleanup)
        self.addCleanup(lambda: setattr(BG, "REPO", self._saved))

    def write_src(self, rel, text):
        return put(self.root / rel, text)

    def write_manifest(self, *rows):
        self.man.write_text("# a comment\n\n" + "".join(f"{r}\n" for r in rows),
                            encoding="utf-8")

    def test_parses_rows_and_ignores_comments(self):
        h = self.write_src("src/f.c", BROKEN)
        self.write_manifest(f"{h} src/f.c extern-c-clash",
                            "# trailing comment line")
        rows = BG.manifest_rows(self.man)
        self.assertEqual(len(rows), 1)
        self.assertEqual(rows[0]["src"], "src/f.c")
        self.assertEqual(rows[0]["reason"], "extern-c-clash")

    def test_malformed_row_raises_rather_than_being_skipped(self):
        """A silently dropped row silently re-inflates the published count."""
        self.write_manifest("deadbeef src/f.c")
        with self.assertRaises(ValueError):
            BG.manifest_rows(self.man)

    def test_exclusion_is_live_while_the_hash_matches(self):
        h = self.write_src("src/f.c", BROKEN)
        self.write_manifest(f"{h} src/f.c extern-c-clash")
        self.assertEqual(BG.excluded_paths(self.man), {"src/f.c"})
        self.assertEqual(BG.stale_rows(self.man), [])

    def test_fixing_the_file_lapses_the_exclusion_with_no_list_edit(self):
        """The self-healing property. Nobody has to remember this file exists."""
        h = self.write_src("src/f.c", BROKEN)
        self.write_manifest(f"{h} src/f.c extern-c-clash")
        self.assertEqual(BG.excluded_paths(self.man), {"src/f.c"})
        self.write_src("src/f.c", FIXED)
        self.assertEqual(BG.excluded_paths(self.man), set())

    def test_an_edited_file_is_reported_stale_not_silently_dropped(self):
        h = self.write_src("src/f.c", BROKEN)
        self.write_manifest(f"{h} src/f.c extern-c-clash")
        fixed = self.write_src("src/f.c", FIXED)
        stale = BG.stale_rows(self.man)
        self.assertEqual([s["problem"] for s in stale], ["changed"])
        self.assertEqual(stale[0]["have"], fixed)

    def test_a_deleted_file_is_reported_missing(self):
        h = self.write_src("src/f.c", BROKEN)
        self.write_manifest(f"{h} src/f.c extern-c-clash")
        (self.root / "src" / "f.c").unlink()
        self.assertEqual([s["problem"] for s in BG.stale_rows(self.man)], ["missing"])
        self.assertEqual(BG.excluded_paths(self.man), set())

    def test_hash_is_over_bytes_so_encoding_cannot_move_it(self):
        """src/ carries non-ASCII in codegen comments and the Windows default is cp1252."""
        text = "// → arrow\nint f(void){return 0;}\n"
        p = self.root / "src" / "u.c"
        p.write_bytes(text.encode("utf-8"))
        self.assertEqual(BG.source_hash(p),
                         hashlib.sha256(text.encode("utf-8")).hexdigest()[:BG.HASH_LEN])

    def test_hash_is_the_same_for_crlf_and_lf(self):
        """The bug this normalisation was added for, and the one that would hurt most.

        .gitattributes has no `text=auto`, so these files are CRLF in a stock Windows
        clone and LF on the ubuntu runner that publishes the count. A raw-byte pin
        recorded on Windows misses on all 18 rows in CI, every exclusion lapses in the
        one place that matters, and the published number silently reverts -- while every
        check on the author's machine still passes."""
        body = "int f(void) { return 0; }\nint g(void) { return 1; }\n"
        lf = self.root / "src" / "lf.c"
        crlf = self.root / "src" / "crlf.c"
        lf.write_bytes(body.encode("utf-8"))
        crlf.write_bytes(body.replace("\n", "\r\n").encode("utf-8"))
        self.assertNotEqual(lf.read_bytes(), crlf.read_bytes())
        self.assertEqual(BG.source_hash(lf), BG.source_hash(crlf))

    def test_a_line_ending_flip_does_not_retire_a_row(self):
        """Same property from the policy side: reflowing line endings is not a fix."""
        h = self.write_src("src/f.c", BROKEN)
        self.write_manifest(f"{h} src/f.c extern-c-clash")
        (self.root / "src" / "f.c").write_bytes(BROKEN.replace("\n", "\r\n").encode())
        self.assertEqual(BG.excluded_paths(self.man), {"src/f.c"})
        self.assertEqual(BG.stale_rows(self.man), [])


class RealManifest(unittest.TestCase):
    """The committed manifest against the committed tree. This is the tripwire."""

    def test_every_row_is_well_formed_and_unique(self):
        rows = BG.manifest_rows()
        self.assertTrue(BG.MANIFEST.is_file(), "bytegate manifest is missing")
        paths = [r["src"] for r in rows]
        self.assertEqual(len(paths), len(set(paths)), "duplicate row")
        for r in rows:
            self.assertTrue(r["src"].startswith("src/"), r)
            self.assertEqual(len(r["sha"]), BG.HASH_LEN, r)
            self.assertIn(r["reason"],
                          {"extern-c-clash", "bare-template-arg", "broken-draft"}, r)

    def test_no_row_is_stale(self):
        """Fails when a listed file is edited without re-running the gate. The
        exclusion has lapsed at that point and the function is being counted again, so
        this is a request to run `python tools/bytegate.py --recheck`, not a nuisance."""
        stale = BG.stale_rows()
        self.assertEqual(
            stale, [],
            "stale byte-gate rows (their exclusions have LAPSED and those functions are "
            "counted matched again): "
            + ", ".join(f"{s['src']} ({s['problem']})" for s in stale)
            + " -- re-run `python tools/bytegate.py --recheck`")

    def test_the_manifest_does_not_list_a_zero_size_alias(self):
        """The two halves must stay disjoint, or the CI line double-counts them."""
        rows = {r["src"] for r in BG.manifest_rows()}
        for name in ("src/runtime/math/_dmul.c", "src/runtime/math/_ll_sdiv.c", "src/runtime/math/_s32_div_f.c",
                     "src/runtime/math/_u32_div_f.c"):
            self.assertNotIn(name, rows, "handled by the derived half, not the manifest")


class AliasCollisions(unittest.TestCase):
    """The derived half, over a fake config tree."""

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = pathlib.Path(self.tmp.name)
        self.addCleanup(self.tmp.cleanup)
        self._saved = RL.module_universe

    def universe(self, text, label="itcm"):
        p = self.root / "symbols.txt"
        p.write_text(text, encoding="utf-8")
        RL.module_universe = lambda: [(p, label)]
        self.addCleanup(lambda: setattr(RL, "module_universe", self._saved))

    def test_zero_size_record_sharing_an_address_with_a_sized_one_is_found(self):
        self.universe(
            "func_01ff8708 kind:function(arm,size=0x6f0) addr:0x01ff8708\n"
            "_dmul kind:function(arm,size=0x0) addr:0x01ff8708\n")
        self.assertEqual(CDB.alias_collision_addresses(), {("itcm", 0x01ff8708)})

    def test_a_lone_zero_size_symbol_is_not_flagged(self):
        self.universe("_solo kind:function(arm,size=0x0) addr:0x01ff9000\n")
        self.assertEqual(CDB.alias_collision_addresses(), set())

    def test_two_sized_symbols_at_one_address_are_not_flagged(self):
        self.universe(
            "a kind:function(arm,size=0x10) addr:0x02000000\n"
            "b kind:function(arm,size=0x10) addr:0x02000000\n")
        self.assertEqual(CDB.alias_collision_addresses(), set())


class MatchedConjunct(unittest.TestCase):
    """End to end through chaos_db_ci.main(): a record that fails the byte gate reads
    matched false, and reads true again once it is fixed.

    Runs the real generator over a fake repository rather than asserting on a predicate,
    because the thing that has to be right is the published record -- `matched`, the
    stats block and the coin tally all at once."""

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.root = pathlib.Path(self.tmp.name)
        self.addCleanup(self.tmp.cleanup)
        (self.root / "src").mkdir()
        (self.root / "config").mkdir()
        self.sym = self.root / "config" / "symbols.txt"
        self.sym.write_text(
            "good kind:function(arm,size=0x10) addr:0x02000000\n"
            "broken kind:function(arm,size=0x20) addr:0x02000010\n"
            "sized kind:function(arm,size=0x100) addr:0x02000030\n"
            "alias kind:function(arm,size=0x0) addr:0x02000030\n", encoding="utf-8")
        self.sha = {}
        for name in ("good", "broken", "sized", "alias"):
            self.sha[name] = put(self.root / "src" / f"{name}.c", FIXED)
        self.man = self.root / "config" / "bytegate-known-failures.txt"

        # A real git history: chaos_db_ci reads authorship from `git log`, and with no
        # repository the author field is simply absent, which would make the coin
        # assertions below vacuous rather than wrong.
        def git(*a):
            subprocess.run(["git", *a], cwd=self.root, check=True, capture_output=True)
        git("init", "-q", "-b", "main")
        # _handle_from takes the email local-part, so this must be matcher@, not m@.
        git("config", "user.email", "matcher@example.com")
        git("config", "user.name", "matcher")
        git("add", "-A")
        git("commit", "-q", "-m", "seed")

        # Every global this touches is restored, MANIFEST included. Leaking that one
        # pointed the RealManifest tests at a deleted temp path, where the committed
        # manifest read as empty and its tripwire passed vacuously -- a green test that
        # was checking nothing, which is worse than a red one.
        saved = (CDB.REPO, BG.REPO, BG.MANIFEST, RL.module_universe, SP.path_for,
                 LYC.delinks_paths)
        CDB.REPO, BG.REPO, BG.MANIFEST = self.root, self.root, self.man
        RL.module_universe = lambda: [(self.sym, "arm9")]
        SP.path_for = lambda n: (self.root / "src" / f"{n}.c"
                                 if (self.root / "src" / f"{n}.c").is_file() else None)
        LYC.delinks_paths = lambda *a, **k: {}

        def restore():
            (CDB.REPO, BG.REPO, BG.MANIFEST, RL.module_universe, SP.path_for,
             LYC.delinks_paths) = saved
        self.addCleanup(restore)

    def generate(self):
        out = self.root / "db.json"
        argv = sys.argv
        sys.argv = ["chaos_db_ci.py", "--out", str(out),
                    "--contrib-out", str(self.root / "contrib.json")]
        try:
            CDB.main()
        finally:
            sys.argv = argv
        db = json.loads(out.read_text(encoding="utf-8"))
        return ({f["name"]: f for f in db["functions"]}, db["stats"],
                json.loads((self.root / "contrib.json").read_text(encoding="utf-8")))

    def test_a_manifest_row_reads_matched_false_and_a_fix_reads_true(self):
        self.man.write_text(f"{self.sha['broken']} src/broken.c extern-c-clash\n",
                            encoding="utf-8")
        recs, stats, contrib = self.generate()
        self.assertFalse(recs["broken"]["matched"])
        self.assertEqual(recs["broken"]["byteGate"], "will-not-build")
        self.assertNotIn("author", recs["broken"], "an uncounted function pays nobody")
        self.assertTrue(recs["good"]["matched"], "the gate must not touch its neighbours")
        before = stats["matchedFunctions"]
        before_coins = {c["login"]: c["coins"] for c in contrib["contributors"]}

        # Fix it. The manifest is NOT edited: the hash no longer matches, so the row
        # stops applying on its own.
        put(self.root / "src" / "broken.c", FIXED + "// fixed\n")
        recs, stats, contrib = self.generate()
        self.assertTrue(recs["broken"]["matched"])
        self.assertNotIn("byteGate", recs["broken"])
        self.assertEqual(stats["matchedFunctions"], before + 1)
        self.assertEqual(stats["matchedBytes"], 0x10 + 0x20 + 0x100)
        after_coins = {c["login"]: c["coins"] for c in contrib["contributors"]}
        self.assertEqual(after_coins["matcher"], before_coins["matcher"] + 1)

    def test_the_zero_size_alias_drops_and_its_sized_twin_is_untouched(self):
        """The address-keying bug: both records sit on 0x02000030 and only the
        zero-size one may be excluded."""
        self.man.write_text("", encoding="utf-8")
        recs, stats, _ = self.generate()
        self.assertFalse(recs["alias"]["matched"])
        self.assertEqual(recs["alias"]["byteGate"], "zero-size-alias")
        self.assertTrue(recs["sized"]["matched"],
                        "the sized primary is the body that SHOULD count")
        self.assertNotIn("byteGate", recs["sized"])
        self.assertEqual(stats["matchedBytes"], 0x10 + 0x20 + 0x100)

    def test_a_lapsed_row_falls_back_to_counting_rather_than_guessing(self):
        """Permissive on staleness, by design: the count returns to the old behaviour
        and RealManifest.test_no_row_is_stale is what makes the lapse loud."""
        self.man.write_text(f"{'0' * BG.HASH_LEN} src/broken.c extern-c-clash\n",
                            encoding="utf-8")
        recs, _, _ = self.generate()
        self.assertTrue(recs["broken"]["matched"])
        self.assertEqual([s["problem"] for s in BG.stale_rows(self.man)], ["changed"])


if __name__ == "__main__":
    unittest.main()
