#!/usr/bin/env python3
"""Tests for tools/source_coverage.py.

THE FAILURE PATH IS WHAT THESE DEFEND. This repo has shipped gates that passed when they
should have failed -- one reported a clean tree after a reformat took its match count to
zero -- so the tests that earn their keep here are the NEGATIVE controls: given a
delinks.txt that has genuinely handed bytes back to the cartridge, does the tool exit
non-zero AND name the range? There is one such test for each of the four ways `dsd`
reclaims a range:

    complete removed        DowngradeTests.test_dropping_complete_is_a_regression
    entry deleted           DowngradeTests.test_deleting_an_entry_is_a_regression
    file missing            DowngradeTests.test_entry_naming_a_missing_file_is_a_regression
    range narrowed          DowngradeTests.test_narrowing_a_range_is_a_regression

Each asserts the exit code AND that the module, address and owning path appear in the
output, because "exits 1" alone is satisfied by a tool that fails on everything.

The matching POSITIVE control is `NoFalseAlarmTests.test_tu_consolidation_is_silent`: a
TU promotion that re-partitions 3 per-function entries into 1 merged entry covering the
same bytes must stay green. A gate that fires on every consolidation PR would be turned
off within a day, so that test is as load-bearing as the four above.

Self-running: `python tools/test_source_coverage.py`, or via unittest/pytest.
Needs git on PATH; needs no ROM and no compiler.
"""
import json
import os
import pathlib
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import source_coverage as SC  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent

# A miniature arm9 delinks.txt: the module header block (which must NOT be counted, it
# describes the module rather than any source file), then four entries -- three complete
# and one deliberately left as a ROM-byte placeholder.
BASE_DELINKS = """\
    .text       start:0x02004000 end:0x02010000 kind:code align:32
    .rodata     start:0x02010000 end:0x02011000 kind:rodata align:4

src/Entry.c:
    complete
    .text start:0x02004800 end:0x02004900

src/func_02004900.c:
    complete
    .text start:0x02004900 end:0x02004a00

src/func_02004a00.c:
    complete
    .text start:0x02004a00 end:0x02004c00

src/placeholder.c:
    .text start:0x02004c00 end:0x02004d00
"""

# 0x100 + 0x100 + 0x200 = 0x400.  placeholder.c has no `complete`, so it is cartridge
# bytes in the base too and must never be counted on either side.
BASE_BYTES = 0x400


class Sandbox:
    """A throwaway git repo shaped like this one: config/arm9/delinks.txt plus src/."""

    def __init__(self, tmp):
        self.root = pathlib.Path(tmp)
        self.git("init", "-q", "-b", "main")
        self.git("config", "user.email", "t@example.invalid")
        self.git("config", "user.name", "t")

    def git(self, *args):
        return subprocess.run(("git", "-C", str(self.root)) + args, check=True,
                              capture_output=True, encoding="utf-8").stdout

    def write(self, rel, text):
        p = self.root / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text, encoding="utf-8", newline="\n")
        return p

    def delinks(self, text):
        self.write("config/arm9/delinks.txt", text)

    def sources(self, *names):
        for n in names:
            self.write(f"src/{n}", "/* stub */\n")

    def commit(self, msg="c"):
        self.git("add", "-A")
        self.git("commit", "-q", "-m", msg)
        return self.git("rev-parse", "HEAD").strip()

    def seed(self):
        """The base state: BASE_DELINKS with every named file present, committed."""
        self.delinks(BASE_DELINKS)
        self.sources("Entry.c", "func_02004900.c", "func_02004a00.c", "placeholder.c")
        return self.commit("base")

    def run(self, *argv, exceptions=None):
        """(exit_code, stdout) from source_coverage.main against this sandbox."""
        args = list(argv) + ["--repo", str(self.root),
                             "--exceptions", exceptions or str(self.root / "none.jsonl")]
        import io
        import contextlib
        buf = io.StringIO()
        err = io.StringIO()
        with contextlib.redirect_stdout(buf), contextlib.redirect_stderr(err):
            code = SC.main(args)
        return code, buf.getvalue() + err.getvalue()


class SandboxCase(unittest.TestCase):
    def setUp(self):
        self._tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self._cleanup)
        self.sb = Sandbox(self._tmp.name)
        self.base = self.sb.seed()

    def _cleanup(self):
        # git leaves read-only objects on Windows; TemporaryDirectory chokes on them.
        try:
            self._tmp.cleanup()
        except (OSError, PermissionError):
            pass

    def check(self, exceptions=None):
        return self.sb.run("--check", "--base", self.base, exceptions=exceptions)


class ParsingTests(unittest.TestCase):
    """The parse decides what counts; get it wrong and every number downstream is wrong."""

    def test_module_header_block_is_not_a_source_range(self):
        """The leading `.text start:...` block has no entry above it and must be dropped.

        Counting it would report the whole 48 KB module as source-built and make every
        subsequent comparison meaningless.
        """
        entries = SC.parse_delinks(BASE_DELINKS, "config/arm9/delinks.txt")
        self.assertEqual([e.path for e in entries],
                         ["src/Entry.c", "src/func_02004900.c",
                          "src/func_02004a00.c", "src/placeholder.c"])

    def test_an_entry_without_complete_is_not_source_built(self):
        """No `complete` means dsd supplies the object -- see tools/enroll.py."""
        entries = SC.parse_delinks(BASE_DELINKS, "config/arm9/delinks.txt")
        by = {e.path: e for e in entries}
        self.assertTrue(by["src/Entry.c"].complete)
        self.assertFalse(by["src/placeholder.c"].complete)
        exists = {e.path for e in entries}
        built = SC.built_ranges(entries, exists)
        self.assertNotIn("src/placeholder.c", {p for _m, _s, p, _a, _b in built})
        self.assertEqual(sum(b - a for _m, _s, _p, a, b in built), BASE_BYTES)

    def test_module_name_comes_from_the_delinks_path(self):
        self.assertEqual(SC.module_of("config/arm9/delinks.txt"), "arm9")
        self.assertEqual(
            SC.module_of("config/arm9/overlays/ov010/delinks.txt"), "ov010")

    def test_every_named_section_counts_not_just_text(self):
        """`.init` is kind:code and is compiled from src/ exactly like `.text`.

        305 entries in the real tree name `.init`; a .text-only tool would let all of
        them be handed back in silence.
        """
        entries = SC.parse_delinks(
            "src/__sinit.c:\n    complete\n    .init start:0x02073a24 end:0x02073e6c\n",
            "config/arm9/delinks.txt")
        built = SC.built_ranges(entries, {"src/__sinit.c"})
        self.assertEqual([(m, s) for m, s, _p, _a, _b in built], [("arm9", ".init")])
        self.assertEqual(SC.by_section(SC.merge_intervals(built))[".init"], 0x448)


class IntervalTests(unittest.TestCase):
    """The byte-set arithmetic, which is the only unit that survives a re-partition."""

    def test_adjacent_ranges_merge(self):
        got = SC.merge_intervals([("m", ".text", "a", 0, 10), ("m", ".text", "b", 10, 20)])
        self.assertEqual(got[("m", ".text")], [(0, 20)])

    def test_sections_do_not_merge_across_each_other(self):
        got = SC.merge_intervals([("m", ".text", "a", 0, 10), ("m", ".init", "b", 0, 10)])
        self.assertEqual(sorted(got), [("m", ".init"), ("m", ".text")])

    def test_subtract_reports_only_the_uncovered_middle(self):
        a = SC.merge_intervals([("m", ".text", "a", 0, 100)])
        b = SC.merge_intervals([("m", ".text", "b", 0, 40), ("m", ".text", "c", 60, 100)])
        self.assertEqual(SC.subtract(a, b), [("m", ".text", 40, 60)])


class NoFalseAlarmTests(SandboxCase):
    """Green must mean green. A gate that cries wolf gets switched off."""

    def test_an_unchanged_tree_is_clean(self):
        code, out = self.check()
        self.assertEqual(code, 0, out)
        self.assertIn("clean", out)

    def test_tu_consolidation_is_silent(self):
        """Three per-function entries become one merged TU covering the same bytes.

        This is the single most common shape of PR in this repo. Comparing (path, start,
        end) tuples would call it 3 removals and 1 addition; comparing the covered BYTE
        SET correctly calls it nothing at all.
        """
        self.sb.delinks(BASE_DELINKS.split("src/Entry.c:")[0] + """\
src/Merged.cpp:
    complete
    .text start:0x02004800 end:0x02004c00

src/placeholder.c:
    .text start:0x02004c00 end:0x02004d00
""")
        for n in ("Entry.c", "func_02004900.c", "func_02004a00.c"):
            os.remove(self.sb.root / "src" / n)
        self.sb.sources("Merged.cpp")
        code, out = self.check()
        self.assertEqual(code, 0, out)
        self.assertIn("net +0 B", out)

    def test_a_pure_gain_is_clean(self):
        """Promoting the placeholder to `complete` is an increase and must pass."""
        self.sb.delinks(BASE_DELINKS.replace(
            "src/placeholder.c:\n    .text", "src/placeholder.c:\n    complete\n    .text"))
        code, out = self.check()
        self.assertEqual(code, 0, out)
        self.assertIn("newly built from source: 256", out)


class DowngradeTests(SandboxCase):
    """One test per way `dsd` reclaims a range. These are the point of the file.

    Every one asserts the exit code AND that the output names the module, the address and
    the file that used to own it: "exits 1" on its own is satisfied by a tool that fails
    unconditionally, and a bare byte count is not actionable.
    """

    def assertRegressed(self, out, code, *, addr, path, why):
        self.assertEqual(code, 1, f"gate PASSED when it should have failed:\n{out}")
        self.assertIn("REGRESSION", out)
        self.assertIn("arm9", out)
        self.assertIn(addr, out)
        self.assertIn(path, out)
        self.assertIn(why, out)

    def test_dropping_complete_is_a_regression(self):
        """The entry and the file both still exist; only `complete` is gone.

        Invisible to layout_check.py -- its L1 keys on the entry header and never reads
        the `complete` line -- and invisible to the ROM build, which stays byte-perfect.
        """
        self.sb.delinks(BASE_DELINKS.replace(
            "src/func_02004a00.c:\n    complete\n", "src/func_02004a00.c:\n", 1))
        code, out = self.check()
        self.assertRegressed(out, code, addr="0x02004a00",
                             path="src/func_02004a00.c", why="`complete` removed")
        self.assertIn("512", out)          # 0x02004a00-0x02004c00

    def test_deleting_an_entry_is_a_regression(self):
        """The entry is gone from delinks.txt; the source file is untouched on disk.

        layout_check.py sees this as L5 "unenrolled", which is INFO by design and can
        never fail anything.
        """
        self.sb.delinks(BASE_DELINKS.replace(
            "src/func_02004900.c:\n    complete\n"
            "    .text start:0x02004900 end:0x02004a00\n\n", "", 1))
        code, out = self.check()
        self.assertRegressed(out, code, addr="0x02004900",
                             path="src/func_02004900.c",
                             why="entry no longer in delinks.txt")

    def test_entry_naming_a_missing_file_is_a_regression(self):
        """A move that did not carry into delinks.txt: `complete`, but nothing there.

        This one layout_check.py DOES catch, as L1. It is here because the byte total
        must agree with L1 rather than credit a range no compiler will ever see.
        """
        os.remove(self.sb.root / "src" / "Entry.c")
        code, out = self.check()
        self.assertRegressed(out, code, addr="0x02004800", path="src/Entry.c",
                             why="no file is there")

    def test_narrowing_a_range_is_a_regression(self):
        """`end:` pulled back. layout_check.py parses no addresses and cannot see it."""
        self.sb.delinks(BASE_DELINKS.replace(
            ".text start:0x02004a00 end:0x02004c00",
            ".text start:0x02004a00 end:0x02004b00", 1))
        code, out = self.check()
        self.assertRegressed(out, code, addr="0x02004b00",
                             path="src/func_02004a00.c", why="range narrowed")

    def test_a_gain_does_not_offset_a_loss(self):
        """A net-positive PR that also drops a range must still fail, and name it.

        This is the count-ratchet trap the CONVERTED ratchet documents: a total that only
        has to not fall is satisfied by wrecking one range while adding a bigger one.
        """
        self.sb.delinks(BASE_DELINKS
                        .replace("src/Entry.c:\n    complete\n", "src/Entry.c:\n", 1)
                        .replace("src/placeholder.c:\n    .text",
                                 "src/placeholder.c:\n    complete\n    .text"))
        code, out = self.check()
        self.assertRegressed(out, code, addr="0x02004800", path="src/Entry.c",
                             why="`complete` removed")

    def test_json_mode_also_reports_the_regression(self):
        """--json must not be a way to get a zero exit out of a failing tree."""
        self.sb.delinks(BASE_DELINKS.replace(
            "src/func_02004a00.c:\n    complete\n", "src/func_02004a00.c:\n", 1))
        code, out = self.sb.run("--check", "--base", self.base, "--json",
                                exceptions=str(self.sb.root / "none.jsonl"))
        self.assertEqual(code, 1, out)
        body = json.loads(out)
        self.assertEqual(body["unwaivedLostBytes"], 512)
        self.assertEqual(body["lost"][0]["start"], "0x2004a00")
        self.assertEqual(body["lost"][0]["owners"][0]["path"], "src/func_02004a00.c")


class WaiverTests(SandboxCase):
    """The escape hatch has to work, and has to refuse to be used silently."""

    def _regress(self):
        self.sb.delinks(BASE_DELINKS.replace(
            "src/func_02004a00.c:\n    complete\n", "src/func_02004a00.c:\n", 1))

    def test_a_waived_range_passes_and_prints_the_reason(self):
        self._regress()
        w = self.sb.write("w.jsonl", json.dumps({
            "module": "arm9", "section": ".text", "start": "0x02004a00",
            "end": "0x02004c00", "reason": "fakematch retracted in #1234"}) + "\n")
        code, out = self.check(exceptions=str(w))
        self.assertEqual(code, 0, out)
        self.assertIn("fakematch retracted in #1234", out)
        self.assertNotIn("REGRESSION", out)

    def test_a_waiver_with_no_reason_is_rejected_not_honoured(self):
        """An unexplained waiver is this gate's own failure mode wearing a hat."""
        self._regress()
        w = self.sb.write("w.jsonl", json.dumps({
            "module": "arm9", "section": ".text",
            "start": "0x02004a00", "end": "0x02004c00"}) + "\n")
        code, out = self.check(exceptions=str(w))
        self.assertEqual(code, 2, out)
        self.assertIn("reason", out)

    def test_a_malformed_waiver_file_is_a_configuration_error(self):
        """Not: silently drop the bad line and carry on with a weakened gate."""
        self._regress()
        w = self.sb.write("w.jsonl", "{not json\n")
        code, out = self.check(exceptions=str(w))
        self.assertEqual(code, 2, out)

    def test_a_waiver_does_not_cover_a_different_range(self):
        """Waivers are per-range, so one cannot be reused to hide the next regression."""
        self._regress()
        w = self.sb.write("w.jsonl", json.dumps({
            "module": "arm9", "section": ".text", "start": "0x02008000",
            "end": "0x02009000", "reason": "unrelated"}) + "\n")
        code, out = self.check(exceptions=str(w))
        self.assertEqual(code, 1, out)


class UsageTests(SandboxCase):
    """Modes that must not fail open."""

    def test_check_without_a_base_is_a_usage_error(self):
        """Not exit 0. `--check` with nothing to compare against gates nothing."""
        code, out = self.sb.run("--check")
        self.assertEqual(code, 2, out)

    def test_an_unresolvable_base_is_an_error_by_default(self):
        code, out = self.sb.run("--check", "--base", "0" * 40)
        self.assertEqual(code, 2, out)

    def test_allow_empty_base_skips_a_first_push(self):
        """github.event.before is forty zeros for a branch's first push."""
        code, out = self.sb.run("--check", "--base", "0" * 40, "--allow-empty-base")
        self.assertEqual(code, 0, out)
        self.assertIn("skipping", out)

    def test_merge_base_mode_finds_the_fork_point(self):
        """A branch is judged against where it FORKED, not against a moved main.

        Measured on the real tree: comparing three branches against current main
        invented a 4,028 B loss that was zero against their own merge bases.
        """
        self.sb.git("checkout", "-q", "-b", "feature")
        self.sb.write("src/new.c", "/* new */\n")
        self.sb.delinks(BASE_DELINKS + """
src/new.c:
    complete
    .text start:0x02005000 end:0x02005100
""")
        self.sb.commit("branch adds a range")
        self.sb.git("checkout", "-q", "main")
        self.sb.write("src/other.c", "/* main moved on */\n")
        self.sb.delinks(BASE_DELINKS + """
src/other.c:
    complete
    .text start:0x02006000 end:0x02006400
""")
        self.sb.commit("main moves on")
        self.sb.git("checkout", "-q", "feature")
        # Against current main the branch looks like it lost other.c's 1024 B.
        code, out = self.sb.run("--check", "--base", "main")
        self.assertEqual(code, 1, out)
        # Against its own merge base it lost nothing and gained 256 B.
        code, out = self.sb.run("--check", "--merge-base", "main")
        self.assertEqual(code, 0, out)
        self.assertIn("newly built from source: 256", out)


class RealTreeTests(unittest.TestCase):
    """One end-to-end pass over the actual repo, so the parse cannot rot unnoticed."""

    def test_the_real_tree_reports_a_plausible_total(self):
        entries, exists = SC.entries_from_tree(REPO)
        built = SC.built_ranges(entries, exists)
        secs = SC.by_section(SC.merge_intervals(built))
        # Loose bounds on purpose: this asserts the parse still WORKS, not a value that
        # every enrollment PR would have to update. A parse that broke would read zero.
        self.assertGreater(secs.get(".text", 0), 1_500_000)
        self.assertGreater(secs.get(".init", 0), 50_000)
        self.assertGreater(len({m for m, _s, _p, _a, _b in built}), 50)


if __name__ == "__main__":
    unittest.main(verbosity=2)
