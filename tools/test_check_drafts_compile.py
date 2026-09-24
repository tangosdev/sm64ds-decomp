"""Regression tests for tools/check_drafts_compile.py.

Each case plants the breakage and asserts the gate goes red. Like
test_check_src_tu_compiles.py, NO TEST HERE SKIPS ITSELF: the compiling cases need
mwccarm and fail loudly without it, because a compile gate whose test passes without a
compiler is the false green the gate exists for. So this module is deliberately not in
tool-tests.yml's list; run it on a box with the toolchain.
"""
import pathlib
import shutil
import sys
import tempfile
import unittest
import unittest.mock

TOOLS = pathlib.Path(__file__).resolve().parent
REPO = TOOLS.parent
sys.path.insert(0, str(TOOLS))

import build_pin as BP              # noqa: E402
import check_drafts_compile as CD   # noqa: E402

BANNER = ("// NONMATCHING: probe draft (div=1). Logic verified correct vs ROM; not\n"
          "// byte-matchable from C at mwccarm 2004/b56.\n")
GOOD = BANNER + "int probe_draft(int a) { return a + 1; }\n"
# An undeclared identifier: the shape of a draft stranded by a header change it
# never saw.
BROKEN = BANNER + "int probe_draft(int a) { return a + probe_gone; }\n"
UNBANNERED = "int probe_matched(int a) { return a; }\n"


class Scratch:
    """Throwaway sources under build/, so the gate's paths stay repo-relative."""

    def __enter__(self):
        (REPO / "build").mkdir(exist_ok=True)
        self.dir = pathlib.Path(tempfile.mkdtemp(prefix="drafts-", dir=REPO / "build"))
        return self

    def write(self, name, text):
        p = self.dir / name
        p.write_text(text, encoding="utf-8")
        return p.relative_to(REPO).as_posix()

    def __exit__(self, *exc):
        shutil.rmtree(self.dir, ignore_errors=True)


class WorklistTests(unittest.TestCase):
    def test_only_bannered_files_are_drafts(self):
        with Scratch() as s:
            draft = s.write("func_draft.c", GOOD)
            s.write("func_matched.c", UNBANNERED)
            s.write("notes.txt", GOOD)
            self.assertEqual(CD.drafts(s.dir), [draft])

    def test_an_empty_worklist_fails(self):
        with Scratch() as s, unittest.mock.patch.object(CD, "drafts", return_value=[]):
            report = CD.check()
            self.assertFalse(report["ok"])
            self.assertEqual(report["drafts"], 0)


class CompileTests(unittest.TestCase):
    def test_a_draft_that_compiles_passes(self):
        with Scratch() as s:
            report = CD.check([s.write("func_draft.c", GOOD)])
            self.assertTrue(report["ok"], report["failures"])
            self.assertEqual(report["compiled"], 1)

    def test_a_draft_that_no_longer_compiles_fails(self):
        with Scratch() as s:
            rel = s.write("func_draft.c", BROKEN)
            report = CD.check([rel])
            self.assertFalse(report["ok"])
            self.assertEqual([f[0] for f in report["failures"]], [rel])

    def test_a_missing_compiler_is_a_failure_not_a_skip(self):
        with Scratch() as s, unittest.mock.patch.object(
                BP, "compiler_for", return_value=(None, "pinned compiler is not installed")):
            report = CD.check([s.write("func_draft.c", GOOD)])
            self.assertFalse(report["ok"])
            self.assertEqual(report["compiled"], 0)

    def test_the_committed_tree_compiles(self):
        report = CD.check()
        self.assertGreater(report["drafts"], 0)
        self.assertTrue(report["ok"], report["failures"])


if __name__ == "__main__":
    unittest.main()
