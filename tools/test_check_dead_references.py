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

    def test_a_dead_reference_in_a_cpp_line_comment_is_detected(self):
        dead = self._dead(lambda t: t.write(
            "src/thing.cpp", "int x; // promoted from src/vanished.cpp\n"))
        self.assertIn(("src/thing.cpp", "src/vanished.cpp"), dead)

    def test_a_dead_reference_in_a_multiline_header_comment_is_detected(self):
        dead = self._dead(lambda t: t.write(
            "include/thing.h", "/*\n * See src/vanished.c for the old body.\n */\n"))
        self.assertIn(("include/thing.h", "src/vanished.c"), dead)

    def test_cpp_strings_and_includes_are_not_comments(self):
        dead = self._dead(lambda t: t.write(
            "src/thing.cpp",
            "#include \"include/vanished.h\"\n"
            "const char* a = \"src/vanished.cpp\";\n"
            "const char* b = R\"tag(src/also-vanished.cpp // not prose)tag\";\n"))
        self.assertEqual(dead, set())

    def test_a_live_path_in_a_cpp_comment_is_not_reported(self):
        def build(t):
            t.write("src/legacy.cpp", "int legacy;\n")
            t.write("src/promoted.cpp", "// promoted from src/legacy.cpp\n")
        self.assertEqual(self._dead(build), set())


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


class RelativeLinkTests(unittest.TestCase):
    """A markdown link resolves against ITS OWN DIRECTORY, not the repo root.

    The positive control here is `test_the_2036_shape_is_detected`: the exact link that
    landed in the EAD cross-reference note with all four checks green, because the
    repo-rooted scan resolved the target from the ROOT and found it there. If that test
    ever starts passing vacuously, this gate is back where it was before #2037.
    """

    def _broken(self, build, dead=()):
        with tempfile.TemporaryDirectory() as tmp:
            t = Tree(tmp)
            build(t)
            old = CDR.REPO
            CDR.REPO = t.root
            try:
                _links, broken = CDR.broken_links(t.root, dead)
            finally:
                CDR.REPO = old
            return set(broken)

    def _targets(self, build):
        with tempfile.TemporaryDirectory() as tmp:
            t = Tree(tmp)
            build(t)
            old = CDR.REPO
            CDR.REPO = t.root
            try:
                links = CDR.collect_links(t.root)
            finally:
                CDR.REPO = old
            return [target for _f, _l, target, _r in links]

    def test_the_2036_shape_is_detected(self):
        """A link naming `config/...` from inside `notes/` points under `notes/`."""
        def build(t):
            t.write("config/rom-name-glossary.json", "{}\n")
            t.write("notes/crossref.md",
                    "Bumped in [the glossary](config/rom-name-glossary.json).\n")
        self.assertEqual(self._broken(build), {
            ("notes/crossref.md", 1, "config/rom-name-glossary.json",
             "notes/config/rom-name-glossary.json")})

    def test_a_correct_dotdot_link_resolves(self):
        def build(t):
            t.write("config/rom-name-glossary.json", "{}\n")
            t.write("notes/crossref.md",
                    "Bumped in [the glossary](../config/rom-name-glossary.json).\n")
        self.assertEqual(self._broken(build), set())

    def test_a_sibling_link_resolves(self):
        def build(t):
            t.write("notes/other.md", "x\n")
            t.write("notes/crossref.md", "See [the other note](other.md).\n")
        self.assertEqual(self._broken(build), set())

    def test_an_http_link_is_ignored(self):
        self.assertEqual(self._broken(lambda t: t.write("notes/n.md", (
            "Filed as [an issue](https://github.com/tangosdev/sm64ds-decomp/issues/2037)\n"
            "and raised with [a human](mailto:someone@example.com).\n"))), set())

    def test_an_anchor_only_link_is_ignored(self):
        self.assertEqual(self._broken(lambda t: t.write(
            "notes/n.md", "Jump to [the ratchet](#how-it-avoids-crying-wolf).\n")), set())

    def test_a_fragment_is_stripped_before_resolving(self):
        def build(t):
            t.write("notes/other.md", "x\n")
            t.write("notes/n.md", "See [the section](other.md#the-section).\n")
        self.assertEqual(self._broken(build), set())

    def test_a_link_to_a_directory_resolves(self):
        def build(t):
            t.write("include/dEnemyBase_c.h", "struct dEnemyBase_c {};\n")
            t.write("notes/n.md", "The headers live in [include](../include).\n")
        self.assertEqual(self._broken(build), set())

    def test_a_link_inside_a_fence_is_not_a_link(self):
        """Shown, not followed -- unlike a repo-rooted path in the same fence."""
        self.assertEqual(self._broken(lambda t: t.write("notes/n.md", (
            "Write it like this:\n\n"
            "```markdown\n"
            "[the glossary](config/rom-name-glossary.json)\n"
            "```\n"))), set())

    def test_a_link_inside_backticks_is_not_a_link(self):
        """Hand-rolled dispatch reads exactly like a link and is not one."""
        self.assertEqual(self._broken(lambda t: t.write(
            "notes/n.md",
            "Hand-rolling it as `(*(fn**)this)[4](this, ...)` reproduces the\n"
            "instructions but not the register.\n")), set())

    def test_angle_bracket_and_titled_targets_are_unwrapped(self):
        def build(t):
            t.write("notes/a b.md", "x\n")
            t.write("notes/other.md", "x\n")
            t.write("notes/n.md",
                    "See [spaced](<a b.md>) and [titled](other.md \"the note\").\n")
        self.assertEqual(self._broken(build), set())

    def test_a_reference_style_definition_is_resolved(self):
        def build(t):
            t.write("config/rom-name-glossary.json", "{}\n")
            t.write("notes/n.md",
                    "Cited as [glossary].\n\n[glossary]: config/rom-name-glossary.json\n")
        self.assertEqual(self._broken(build), {
            ("notes/n.md", 3, "config/rom-name-glossary.json",
             "notes/config/rom-name-glossary.json")})

    def test_a_repo_rooted_dead_link_is_not_reported_twice(self):
        """A link to a path that is dead BOTH ways belongs to the repo-rooted scan."""
        build = lambda t: t.write(
            "notes/n.md", "See [the header](../include/Enemy.h).\n")
        self.assertEqual(self._broken(build), {
            ("notes/n.md", 1, "../include/Enemy.h", "include/Enemy.h")})
        self.assertEqual(self._broken(build, dead=[("notes/n.md", "include/Enemy.h")]),
                         set())

    def test_link_syntax_in_python_is_not_a_link(self):
        """A regex character class followed by a group is not a markdown link."""
        self.assertEqual(self._targets(lambda t: t.write(
            "tools/thing.py", '"""Matches [a-z]+(?:pp)? in the name."""\n')), [])

    def test_the_tree_has_no_broken_relative_links(self):
        """Fail-closed: this check banks nothing, so the tree itself must be clean."""
        _files, _refs, dead = CDR.dead_references()
        links, broken = CDR.broken_links(REPO, dead)
        self.assertGreaterEqual(len(links), CDR.MIN_LINKS)
        self.assertEqual(broken, [], f"broken relative links: {broken}")


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
        code_files = [f for f in files if f.endswith(CDR.CODE_SUFFIXES)]
        self.assertGreaterEqual(len(code_files), CDR.MIN_CODE_FILES)
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

    def test_the_code_baseline_is_wellformed_sorted_and_current(self):
        data = json.loads(CDR.CODE_BASELINE.read_text(encoding="utf-8"))
        files = list(data["known"])
        pairs = [(file, ref)
                 for file, refs in data["known"].items() for ref in refs]
        self.assertEqual(files, sorted(files))
        self.assertEqual(pairs, sorted(pairs))
        self.assertEqual(len(pairs), len(set(pairs)))
        for file, _ref in pairs:
            self.assertTrue(file.endswith(CDR.CODE_SUFFIXES))
            self.assertTrue((REPO / file).exists(),
                            f"code baseline names a citing file that is gone: {file}")

    def test_code_baseline_writer_cannot_bank_markdown_debt(self):
        with tempfile.TemporaryDirectory() as tmp:
            old = CDR.CODE_BASELINE
            CDR.CODE_BASELINE = pathlib.Path(tmp) / "code.json"
            try:
                CDR.write_code_baseline({
                    ("src/live.cpp", "src/gone.cpp"),
                    ("notes/live.md", "src/also-gone.cpp"),
                })
                data = json.loads(CDR.CODE_BASELINE.read_text(encoding="utf-8"))
            finally:
                CDR.CODE_BASELINE = old
        self.assertEqual(data["known"], {
            "src/live.cpp": ["src/gone.cpp"],
        })


if __name__ == "__main__":
    unittest.main(verbosity=2)
