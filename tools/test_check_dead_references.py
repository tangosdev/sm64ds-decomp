"""Tests for tools/check_dead_references.py.

The point of this gate is to catch a rename that did not carry into the prose, so the
tests that matter are the POSITIVE CONTROLS: given a docstring that names a file which
is not there, does the tool actually say so? A reference-integrity gate that quietly
finds nothing is worse than no gate at all -- this tree has shipped one of those before
-- so `test_a_dead_reference_is_detected` and `test_a_scan_that_found_nothing_fails`
exist to make a vacuous pass impossible.

Self-running: `python tools/test_check_dead_references.py`, or via unittest/pytest.
Needs no ROM and no compiler.
"""
import json
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import check_dead_references as CDR  # noqa: E402

REPO = pathlib.Path(__file__).resolve().parent.parent


class Tree:
    """A throwaway repo-shaped tree to point the collector at."""

    def __init__(self, tmp):
        self.root = pathlib.Path(tmp)
        (self.root / "tools").mkdir(parents=True, exist_ok=True)
        (self.root / "notes").mkdir(parents=True, exist_ok=True)
        (self.root / "include").mkdir(parents=True, exist_ok=True)

    def write(self, rel, text):
        p = self.root / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text, encoding="utf-8", newline="\n")
        return p


class DetectionTests(unittest.TestCase):
    """Positive controls: the tool must actually find things."""

    def _dead(self, build):
        with tempfile.TemporaryDirectory() as tmp:
            t = Tree(tmp)
            build(t)
            old = CDR.REPO
            CDR.REPO = t.root
            try:
                _files, _refs, dead = CDR.dead_references(t.root)
            finally:
                CDR.REPO = old
            return {(f, r) for f, r in dead}

    def test_a_dead_reference_is_detected(self):
        """A docstring naming a header that is not there must be reported."""
        dead = self._dead(lambda t: t.write(
            "tools/thing.py", '"""See include/Enemy.h for the layout."""\n'))
        self.assertIn(("tools/thing.py", "include/Enemy.h"), dead)

    def test_a_live_reference_is_not_reported(self):
        def build(t):
            t.write("include/dEnemyBase_c.h", "struct dEnemyBase_c {};\n")
            t.write("tools/thing.py", '"""See include/dEnemyBase_c.h for the layout."""\n')
        self.assertEqual(self._dead(build), set())

    def test_a_dead_reference_in_a_comment_is_detected(self):
        dead = self._dead(lambda t: t.write(
            "tools/thing.py", "x = 1  # cf. notes/gone.md\n"))
        self.assertIn(("tools/thing.py", "notes/gone.md"), dead)

    def test_a_dead_reference_in_argparse_help_is_detected(self):
        dead = self._dead(lambda t: t.write("tools/thing.py", (
            "import argparse\n"
            "ap = argparse.ArgumentParser()\n"
            "ap.add_argument('--x', help='writes tools/vanished.py')\n")))
        self.assertIn(("tools/thing.py", "tools/vanished.py"), dead)

    def test_a_dead_reference_in_a_note_is_detected(self):
        dead = self._dead(lambda t: t.write(
            "notes/live.md", "The rule is stated in `include/ClsnResult.h`.\n"))
        self.assertIn(("notes/live.md", "include/ClsnResult.h"), dead)

    def test_a_string_literal_is_not_prose(self):
        """Only docstrings, comments and argparse help count -- not ordinary code."""
        dead = self._dead(lambda t: t.write(
            "tools/thing.py", "OPEN = 'include/Enemy.h'\n"))
        self.assertEqual(dead, set())


class NoiseTests(unittest.TestCase):
    """Negative controls: the shapes that must never be called a path."""

    def _dead(self, body):
        with tempfile.TemporaryDirectory() as tmp:
            t = Tree(tmp)
            t.write("tools/thing.py", f'"""{body}"""\n')
            old = CDR.REPO
            CDR.REPO = t.root
            try:
                _f, _r, dead = CDR.dead_references(t.root)
            finally:
                CDR.REPO = old
            return {r for _f, r in dead}

    def test_slash_separated_word_pairs_are_not_paths(self):
        for body in ("ctor/dtor", "if/else", "106/106", "bytes/relocs", "D0/D1/D2"):
            with self.subTest(body=body):
                self.assertEqual(self._dead(body), set())

    def test_generated_roots_are_skipped(self):
        for body in ("build/tu_map.json", "extracted/dsd/config.yaml",
                     "progress/matched.jsonl"):
            with self.subTest(body=body):
                self.assertEqual(self._dead(body), set())

    def test_globs_and_placeholders_are_skipped(self):
        for body in ("tools/**/*.py", "include/<Class>.h", "config/{a,b}/x.txt"):
            with self.subTest(body=body):
                self.assertEqual(self._dead(body), set())

    def test_a_non_repo_first_segment_is_skipped(self):
        self.assertEqual(self._dead("vendor/m2c/main.py"), set())


class FailLoudlyTests(unittest.TestCase):
    """A scan that inspected nothing must not print a pass."""

    def test_a_scan_that_found_nothing_fails(self):
        with tempfile.TemporaryDirectory() as tmp:
            old_repo, old_base = CDR.REPO, CDR.BASELINE
            CDR.REPO = pathlib.Path(tmp)
            CDR.BASELINE = pathlib.Path(tmp) / "config" / "baseline.json"
            try:
                rc = CDR.main([])
            finally:
                CDR.REPO, CDR.BASELINE = old_repo, old_base
        self.assertEqual(rc, 2, "an empty scan must exit non-zero, not report clean")

    def test_the_floors_are_below_the_real_tree(self):
        files, refs, _dead = CDR.dead_references()
        self.assertGreaterEqual(len(files), CDR.MIN_FILES)
        self.assertGreaterEqual(len(refs), CDR.MIN_REFS)


class BaselineTests(unittest.TestCase):

    def test_the_committed_baseline_matches_the_tree(self):
        """The gate passes on this checkout -- and passes because it ran, not because
        it found nothing."""
        files, refs, dead = CDR.dead_references()
        self.assertGreater(len(files), 0)
        self.assertGreater(len(refs), 0)
        known = CDR.load_baseline()
        new = sorted(set(dead) - known)
        self.assertEqual(new, [], f"unbaselined dead references: {new}")

    def test_the_baseline_is_wellformed(self):
        data = json.loads(CDR.BASELINE.read_text(encoding="utf-8"))
        self.assertIn("known", data)
        for e in data["known"]:
            self.assertEqual(set(e), {"file", "ref"})
            self.assertTrue((REPO / e["file"]).exists(),
                            f"baseline names a citing file that is gone: {e['file']}")

    def test_the_baseline_is_sorted_and_unique(self):
        data = json.loads(CDR.BASELINE.read_text(encoding="utf-8"))
        pairs = [(e["file"], e["ref"]) for e in data["known"]]
        self.assertEqual(pairs, sorted(pairs))
        self.assertEqual(len(pairs), len(set(pairs)))


if __name__ == "__main__":
    unittest.main(verbosity=2)
